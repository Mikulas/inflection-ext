Czech inclension (declension), české skloňování
===============================================

This extension should provide same functionality and API
as [this PHP implementation](https://github.com/Mikulas/inflection). Do note however that
c extension performs way better.

PHP Extension
=============

Installation
------------

Download this extension from Releases.
OR
Build it on your own.

Dependencies:
	- [phpcpp](http://www.php-cpp.com/)
	- [pcre-8.36](http://www.pcre.org/), build with `--enable-jit --enable-utf8 --enable-unicode-properties`

After installing deps, build extension with
```
make
```

Move the built library into your php `extension_dir` (e.g. `/usr/local/php5/lib/php/extensions/no-debug-non-zts-20131226`)

Finally, enable the extension by copying `inflection.ini` to appropriate ini location.

Usage
-----


```php
<?php

$inflection = new Inflection();
$inflected = $inflection->inflect('kožená bunda');

echo "Natrhnul jsem si $inflected[4]";
// Natrhnul jsem si koženou bundu

echo "$inflected[11] jsou nejlepší";
// kožené bundy jsou nejlepší
```

Based on original work by [Pavel Sedlák](http://www.pteryx.net/sklonovani.html), Tomáš Režnar and [Heureka](https://github.com/heureka/inflection).

**This library is about 23 times faster than my implementation in php.**

Links
-----

- http://prirucka.ujc.cas.cz/
- http://www.pteryx.net/sklonovani.html
- https://en.wikipedia.org/wiki/Czech_declension
- https://en.wikipedia.org/wiki/Grammatical_gender

License
-------

GPL 2.1
