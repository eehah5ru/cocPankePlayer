#pragma once

#include <deque>
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

    bool _permute = true;

    bool _isPaused = false;

    vector<VideoPlayerPtr> _players;

    deque<VideoPlayerPtr> _currentMix;

    // VideoPlayerPtr _currentPlayer;

    void shufflePlaylist () {
      if (_permute) {
        shuffle(_players.begin(), _players.end(), mk_random_gen());

        // only if we have more than one video in playlist
        if (_players.size() > 1) {
          while (tryCurrentPlayer() == make_optional(_players.front())) {
            // shuffle players
            shuffle(_players.begin(), _players.end(), mk_random_gen());
          }
        }
      }

      _currentMix.clear();

      for (auto p : _players) {
        _currentMix.push_back(p);
      }

      LOG_MC_NOTICE() << "current playlist:";

      for (auto p: _currentMix) {
        LOG_MC_NOTICE() << p->getMoviePath();
      }

      // _currentMix.insert(_currentMix.begin(), _players.begin(), _players.end());


      // LOG_MC_VERBOSE() << _name << ": new current set to " << _currentPlayer->getMoviePath();
    }

    optional<VideoPlayerPtr> tryCurrentPlayer () {
      if (_currentMix.empty()) {
        return nullopt;
      }
      return _currentMix.front();
    }

    VideoPlayerPtr currentPlayer() {
      if (_currentMix.empty()) {
        throw logic_error("tryijng to get current player from empty mix");
      }

      return _currentMix.front();
    }

    void changePlayer () {
      currentPlayer()->setPosition(0.0);
      currentPlayer()->stop();

      _currentMix.pop_front();

      // getting new  permutation;
      if (_currentMix.empty()) {
        shufflePlaylist();
      }
      currentPlayer()->setPosition(0.0);
      currentPlayer()->stop();
    }

  public:
    //
    // SETUP
    //
    void setup (string name, string &path, bool alpha = false, bool permute = true) {
      _name = name;
      _permute = permute;

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
      ofDirectory sorted = _dir.getSorted();
      // sorted.listDir();

      if (sorted.size() == 0) {
        throw invalid_argument(format("no videos in direcotry '{}'", sorted.path()));
      }

      for (ofFile f : sorted) {
        VideoPlayerPtr player = make_shared<ofVideoPlayer>();

        if (alpha) {
          player->setPixelFormat(OF_PIXELS_RGBA); //(or RGBA)
          // player->setUseTexture(false);
        }

        player->load(f.path());
        // ofGstVideoPlayer p;
        // p.getGstVideoUtils()->getPipeline()->
        _players.push_back(player);
      }

      shufflePlaylist();
    }

    //
    // UPDATE
    //
    void update () {
      if (!currentPlayer()->isLoaded()) {
        return;
      }

      // if (currentPlayer()->isPaused()) {
      //   return;
      // }

      // if (!currentPlayer()->isPlaying()) {
      //   return;
      // }

      if (_isPaused || !isPlaying()) {
        return;
      }

      currentPlayer()->update();

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
      if (!currentPlayer()->isLoaded()) {
        return;
      }

      float scaleF = getScaleToWindowWidthFactor(currentPlayer());
      int dy = (ofGetHeight() - currentPlayer()->getHeight()*scaleF) / 2;

      ofPushMatrix();
      ofScale(scaleF);
      // we-
      currentPlayer()->draw(x, y+dy);

      ofPopMatrix();
    }

    //
    // START
    //
    void start () {
      LOG_MC_VERBOSE() << _name << ": starting video";
      if (isVideoDone()) {
        LOG_MC_VERBOSE() << _name << ": video is done. loading new one";
        changePlayer();
      }
      _isPaused = false;
      currentPlayer()->play();
    }

    //
    // STOP
    //
    void stop () {
      currentPlayer()->stop();
      _isPaused = false;
    }

    //
    // NEXT
    //
    void next () {
      // stop();
      // shufflePlaylist();
      LOG_MC() << "force changing video";
      changePlayer();
    }

    //
    // PREVIOUS
    //
    void previous () {
      LOG_MC() << "force changing video";

      changePlayer();
    }

    //
    // PAUSE
    //
    void pause () {
      // if (currentPlayer()->isPaused()) {
      //   currentPlayer()->play();
      // } else {
      //   currentPlayer()->p
      // }
      // bool p = currentPlayer()->isPaused();
      _isPaused = !_isPaused;
      currentPlayer()->setPaused(_isPaused);
      LOG_MC() << "toggle pause: " << _isPaused;
    }

    //
    // isVideoDone
    //
    bool isVideoDone () {
      if (isForcePaused()) {
        return false;
      }
      return currentPlayer()->getIsMovieDone();
    }

    //
    // check if playlist is over
    //
    bool isPlaylistDone() {
      return _currentMix.empty();
    }

    //
    // IS PLAYING
    //
    bool isPlaying () {
      return !(currentPlayer()->isPaused());
    }

    //
    // IS FORCE PAUSED
    //
    bool isForcePaused () {
      return _isPaused;
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
      _chapters->setup("chapters", chaptersPath, true, false);
      _distortedMaps->setup("distortedMaps", distortedMapsPath, true, true);

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

      if (_currentPlaylist->isForcePaused()) {
        return;
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

    //
    // NEXT
    //
    void next () {
      // _currentPlaylist->next();
      changePlaylist();
      start();
    }

    //
    // PEVIOUS
    //
    void previous () {
      // _currentPlaylist->previous();
      changePlaylist();
      start();
    }


    //
    // PAUSE
    //
    void pause () {
      _currentPlaylist->pause();
    }

  };
}
