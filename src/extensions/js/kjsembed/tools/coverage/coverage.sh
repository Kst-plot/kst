#!/bin/sh

KJSCMD='../../kjscmd'

$KJSCMD classes.js > supported.txt
$KJSCMD unsupported.js < allqobjects.txt > unsupported.txt

