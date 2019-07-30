#!/bin/bash
PORT=14501
python -m http.server $PORT > /dev/null 2>&1 &
