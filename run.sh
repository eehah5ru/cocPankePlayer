#!/bin/bash

make

GST_VAAPI_ALL_DRIVERS=1  DISPLAY=:0 make run
