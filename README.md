Nsis_Unicode-xml_plugin
=======================

This is not my original code, I only changed the code from http://nsis.sourceforge.net/XML_plug-in .

Nsis has 2 versions: ansi and unicode.
When we moved to nsis unicode we couldn't find this plugin's unicode version so I had to change the ansi version.

IMPORTANT NOTE: the plugin can only work on UTF-8 files, the intent was not to make the plugin read utf-16 / ucs2 xml files! the intent was to make it work with nsis unicode.
