$Id: README.txt,v 1.2.2.6.2.3 2008/04/02 15:20:25 fago Exp $

Workflow-ng Module
------------------------
Next generation workflows for drupal.
by Wolfgang Ziegler, nuppla@zites.net


Workflow-ng allows one to customize drupal's built in workflows in a very flexible way.
In short, it's a rule-based event driven action evaluation system.

E.g. this is useful for
• sending customized mails notifiying your users about important changes
• building flexible content publishing workflows
• creating custom redirections
• and a lot more....

Modules may use workflow-ng's API to provide defaults, which can be customized by users.
Users can share their customizations by using the built-in import/export tool.


Installation
-------------

*Before* starting, make sure that you have read at least the introduction - so you know 
at least the basic concepts. You can find it here:
                     
                          http://drupal.org/node/156288                          


 * First install the token module, which is needed by workflow-ng.
   http://drupal.org/project/token
 * Then copy the whole workflow_ng directory to your modules directory and
   activate the workflow-ng and workflow-ng UI modules.
 * You can find the admin interface at /admin/build/workflow-ng.
 

 
Extension Modules
------------------
Workflow-ng can be extended by any module. Some modules are already included with workflow-ng:

 * State Machines
   This module provides configurable state machines. It provides an API for other modules as well as
   an admin interface allowing users to configure their own custom state machines. So it may be used to
   track the state of content (as well as users).
   The modules comes with with workflow-ng integration, so there is an action that allows you to change
   the state of a machine, but it also provides an event, so you can trigger on state machinge changes.
   Furthermode the module comes with views and token module integration.


 * Configurable Content Links
   This module provides configurable content links, that generate events when they are pressed.
   Optionally one can activate scheduling for a link, so that the user can set the date when the
   event will be invoked. This may be used for scheduled execution of arbitrary actions, e.g. one
   can use it to schedule the publishing of content: http://drupal.org/node/175319
   Each link can be configured to toggle between to different link labels. Thanks to the State Machine
   API the current link state is exposed to Views too. Access permissions can be controlled independently
   for each link.

   Note:
   To make date selecting more user friendly, install the JS Calendar of the Javascript Tools modules 
   package: http://drupal.org/node/57285

 * Per-Entity Logs
   In contrast to the action "Log to watchdog" this module provides actions, which allow one to log
   for specific entities, where entites can be users and nodes (content). It also comes with views integation,
   which allows one to easily build UI for listing log entries.
   This may be useful for a lot of things, e.g. you may log if content goes through several states in a workflow.
   
   Note: To list user log entries, you need the usernode module. This is because views is not able to list users
   on its own. 
   
   After installation there are two default views which list all log messages per content and user. It's
   suggested to restrict accesss to these views.


Developer
---------
Developers check out the developer documenation:
http://drupal.org/node/156299
