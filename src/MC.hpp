#pragma once

#include <ranges>
#include "MC.hpp"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofMain.h"
#include "ofFileUtils.h"
#include "ofMath.h"
#include "ofRectangle.h"
#include "ofVideoPlayer.h"
#include "ofGstVideoPlayer.h"
#include <algorithm>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <format>
#include <utility>
#include <vector>

#define LOG_MC_ERROR() ofLogError( "MC" ) << __FUNCTION__ << ": "
#define LOG_MC_WARNING() ofLogWarning( "MC" ) << __FUNCTION__ << ": "
#define LOG_MC_NOTICE() ofLogNotice( "MC" ) << __FUNCTION__ << ": "
#define LOG_MC_VERBOSE() ofLogVerbose( "MC" ) << __FUNCTION__ << ": "
#define LOG_MC() LOG_MC_NOTICE()

namespace mc {
using namespace std;
  class Playlist;

  using Gen = mt19937;

  using VideoPlayerPtr = shared_ptr<ofVideoPlayer>;
  using PlaylistPtr = shared_ptr<Playlist>;

  template<class T>
  using Current = optional<T>;

  using CurrentPlayer = Current<VideoPlayerPtr>;
  using CurrentPlaylist = Current<PlaylistPtr>;

  // Gen mk_random_gen ();
  //
  // get random generator
  //
  inline Gen mk_random_gen () {
    random_device rd;
    Gen g {rd()};
    return g;
  }

  //
  // scale to window width;
  // scaling by width
  //
  template <typename T>
  float getScaleToWindowWidthFactor (shared_ptr<T> o) {
    return ofGetWidth() / static_cast<float>(o->getWidth());
  }

  template <class T>
  float getScaleToWindowWidthFactor (T &o) {
    return ofGetWidth() / static_cast<float>(o.getWidth());
  }



  //
  // Playlist
  //
  class Playlist {
    ofDirectory _dir;

    string _name;

    vector<VideoPlayerPtr> _players;

    VideoPlayerPtr _currentPlayer;

    void setNewCurrent () {
      shuffle(_players.begin(), _players.end(), mk_random_gen());

      // only if we have more than one video in playlist
      if (_players.size() > 1) {

        while (_currentPlayer == _players.front()) {
          // shuffle players
          shuffle(_players.begin(), _players.end(), mk_random_gen());
        }
      }

      _currentPlayer = _players.front();


      _currentPlayer->setPosition(0.0);
      _currentPlayer->stop();

      LOG_MC_VERBOSE() << _name << ": new current set to " << _currentPlayer->getMoviePath();

    }

  public:
    //
    // SETUP
    //
    void setup (string name, string &path, bool alpha = false) {
      _name = name;

      if (path.empty()) {
        throw invalid_argument("path is empty");
      }

      _dir = ofDirectory(ofToDataPath(path));

      if (!_dir.isDirectory()) {
        throw invalid_argument(format("{} is not a dir", path));
      }

      _dir.allowExt("mov");
      _dir.allowExt("mp4");
      _dir.allowExt("webm");


      _dir.listDir();

      if (_dir.size() == 0) {
        throw invalid_argument(format("no videos in direcotry '{}'", _dir.path()));
      }

      for (ofFile f : _dir) {
        VideoPlayerPtr player = make_shared<ofVideoPlayer>();

        if (alpha) {
          player->setPixelFormat(OF_PIXELS_RGBA); //(or RGBA)
          // player->setUseTexture(false);
        }

        player->load(f.path());
        ofGstVideoPlayer p;
        // p.getGstVideoUtils()->getPipeline()->
        _players.push_back(player);
      }

      setNewCurrent();
    }

    //
    // UPDATE
    //
    void update () {
      if (!_currentPlayer->isLoaded()) {
        return;
      }

      if (_currentPlayer->isPaused()) {
        return;
      }

      if (!_currentPlayer->isPlaying()) {
        return;
      }

      _currentPlayer->update();

      // if (_currentPlayer->getIsMovieDone()) {
      //   _currentPlayer->stop();
      //   setNewCurrent();
      // }
    }

    //
    // DRAW
    //
    //
    void draw (int x, int y) {
      if (!_currentPlayer->isLoaded()) {
        return;
      }

      float scaleF = getScaleToWindowWidthFactor(_currentPlayer);
      int dy = (ofGetHeight() - _currentPlayer->getHeight()*scaleF) / 2;

      ofPushMatrix();
      ofScale(scaleF);
      // we-
      _currentPlayer->draw(x, y+dy);

      ofPopMatrix();
    }

    //
    // START
    //
    void start () {
      LOG_MC_VERBOSE() << _name << ": starting video";
      if (isVideoDone()) {
        LOG_MC_VERBOSE() << _name << ": video is done. loading new one";
        setNewCurrent();
      }
      _currentPlayer->play();
    }

    //
    // STOP
    //
    void stop () {
      _currentPlayer->stop();
    }

    //
    // isVideoDone
    //
    bool isVideoDone () {
      return _currentPlayer->getIsMovieDone();
    }

    //
    // IS PLAYING
    //
    bool isPlaying () {
      return _currentPlayer->isPlaying();
    }

  };

  //
  //
  // master of ceremony
  //
  //
  class MC {

    PlaylistPtr _chapters = make_shared<Playlist>();
    PlaylistPtr _distortedMaps = make_shared<Playlist>();

    vector<PlaylistPtr> _playlists;

    PlaylistPtr _currentPlaylist;

    PlaylistPtr getRandomPlaylist() {
      if (ofRandom(0, 1) > 0.5) {
        return _chapters;
      }
      return _distortedMaps;
    }

    void changePlaylist() {
      auto shuffled = views::all(_playlists);

      while(_currentPlaylist == shuffled.front()) {
        shuffle(shuffled.begin(), shuffled.end(), mk_random_gen());
      }

      _currentPlaylist->stop();

      _currentPlaylist = shuffled.front();
    }

  public:
    //
    // SETUP
    //
    void setup(string &&chaptersPath, string &&distortedMapsPath) {
      _chapters->setup("chapters", chaptersPath, false);
      _distortedMaps->setup("distortedMaps", distortedMapsPath, true);

      _currentPlaylist = getRandomPlaylist();

      _playlists.push_back(_chapters);
      _playlists.push_back(_distortedMaps);
    }

    //
    // UPDATE
    //
    void update() {
      _currentPlaylist->update();

      if (_currentPlaylist->isVideoDone()) {
        changePlaylist();
        _currentPlaylist->start();
      }

      if (!_currentPlaylist->isPlaying()) {
        _currentPlaylist->start();
      }
    }

    //
    // DRAW
    //
    void draw(int x, int y) {
      _currentPlaylist->draw(x, y);

    }

    //
    // START
    //
    void start () {
      _currentPlaylist->start();

    }

    //
    // STOP
    //
    void stop () {
      _currentPlaylist->stop();
    }
  };
}
