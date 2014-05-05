#!/bin/bash

bjam toolset=gcc link=static variant=debug
bjam toolset=gcc link=static variant=release
bjam install