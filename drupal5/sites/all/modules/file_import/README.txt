Drupal file_import.module README.txt
==============================================================================

This module can import multiple files and save them as node attachments.
It is very usefull when you are using such modules as slideshow and flashvideo,
and if you need to import dozens of files at once.

Installation
------------------------------------------------------------------------------
 
 Required:
  - Copy the module files to modules/
  - Enable the module as usual from Drupal's admin pages.
  - Make additional settings at admin/settings/file_import

 Optional:
  - Adjust module's access settings at admin/user/access  

Using
------------------------------------------------------------------------------

You can import files from "Content" section of menu (admin/content/file_import)
or from "File import" tab on every node's page (if enabled in settings). There
you will see a table with list of files in import directory. You can edit title
of every file, and perform simple manipulations with file titles like capitalizing
and making RegExp replaces (usefull when you are importing files like
"photo-from-my-vacation.jpg" and whant to make title "Photo from my vacation").
 
Credits / Contact
------------------------------------------------------------------------------

This module was created by Alexandr Shvets (neochief@drupaldance.com).