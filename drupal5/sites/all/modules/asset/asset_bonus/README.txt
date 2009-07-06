The Asset Bonus module uses getid3, if it is available, to display additional
information about the audio/video file.  

Drupal has a policy of not distributing 3rd party software, so you need to
download and install this separately.  The Asset Bonus module adheres to the
model established by the mediafield contrib module in placing getid3 in the
Drupal base directory.  Please follow the instructions below.

1. Download getid3 library from http://getid3.org/ and unpack it to
   '(drupal base)/misc/lib/getid3' direcctory. So, that
   '(drupal base)/misc/lib/getid3/getid3/getid3.php' will be included.

   IMPORTANT: It must be 'misc' directory which already exists in the Drupal
   root, but not a new one within module's directory.
   
2. IMPORTANT: Remove 'demos' and 'helperapps' subdirectories in
   '(drupal base)/misc/lib/getid3' directory for security reasons.

3. Enable asset.module and asset_bonus.module on 'admin/build/modules' page.
