<?php

$i = new Inflection();
var_dump($i->inflect());
var_dump($i->inflect("jaro"));
var_dump($i->inflect("Marek Dítě"));
var_dump($i->inflect("kožená bunda"));
