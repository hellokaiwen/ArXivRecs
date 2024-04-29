#!/bin/bash

$(xmlstarlet sel -N ns="http://www.w3.org/2005/Atom" -t -m "/ns:feed/ns:entry/ns:author/ns:name" -v "." -n response.xml)
