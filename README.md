# CMlyst
CMlyst is a Content Management application built upon Cutelyst with support for Menus, Pages, Blogs and Feeds
 
 Help is welcome :)

## Dependencies
 * Cutelyst 1.7.0

## Configuration
Create an INI file like cmlyst.conf with:

    [Cutelyst]
    DataLocation = /var/tmp/my_site_data
    production = true

Where:
 * DataLocation is the place where images uploads and sqlite database will be placed
 * production when true will preload the theme templates, which is a lot faster but if you are customizing the theme you will need to reload the process
 
## Running
You can run it with cutelyst-wsgi or uWSGI, both have similar command line options, and you should look at their documentation to know their options, the simplest one:

    cutelyst-wsgi --application path/to/libcmlyst.so --http-socket :3000 --ini cmlyst.conf
  
Now point your browser to http://localhost:3000

## Setup
To create the first admin user set the SETUP enviroment variable, run the server and point your browser to http://localhost:3000/.admin.

## Paths
 * http://localhost:3000/.admin  Admin interface
 * http://localhost:3000/.feed RSS feed
 * http://localhost:3000/.author/slug Author page
 
