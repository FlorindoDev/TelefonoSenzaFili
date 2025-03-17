#!/bin/bash

docker build -f Dockerfile.Client -t telefonoclient:1.0.0 .

docker build -t telefono:1.0.0 .