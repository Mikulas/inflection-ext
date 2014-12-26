<?php

echo "\n\033[1;34;mTest:\033[;m\n";

$i = new Inflection();
var_dump($i->inflect("on"));
var_dump($i->inflect("kožená bunda"));
