<?php

echo "\n\033[1;34;mTest:\033[;m\n";

$i = new Inflection();
// var_dump($i->inflect());
var_dump($i->inflect(">ě<"));
// var_dump($i->inflect("ě:"));
// var_dump($i->inflect("Marek Dítě"));
// var_dump($i->inflect("kožená bunda"));
