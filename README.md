Open BGP Monitoring Protocol (OpenBMP) Collector
================================================
![Build Status](http://build-jenkins.openbmp.org/buildStatus/icon?job=openbmp-server-ubuntu-trusty)


OpenBMP is an open source project that implements **draft-ietf-grow-bmp-07**.  BMP protocol version 3 is defined in draft 07, while versions 1 and 2 are defined in the previous revisions of the draft.

JunOS 10.4 implements the older versions of BMP.   Cisco IOS XE 3.12, IOS XR, and JunOS 13.3 implement version 3 (draft 07).


### Daemon
OpenBMP daemon is a BMP receiver for devices that implement BMP, such as Cisco and Juniper routers.  Collected BMP messages are decoded and stored in a SQL database.

> Raw dumps are on the roadmap

### UI
OpenBMP UI is a Web-App UI that interacts with OpenBMP server/daemon.   Multiple OpenBMP daemons can be centrally managed via the same UI.

### Database
The SQL/transactional database is designed to be flexible for all types of reporting on the collected data by simply linking tables or by creating views.

The database is tuned to support high transactional rates and storage for millions of prefixes and other BGP information.   OpenBMP statistics track how well the database is performing and will alert if there are any issues.

News
----
### Mar-27-2015
> #### UPGRADE YOUR SCHEMA if using BGP-LS (link-state)

BGP-LS is now supported.   New tables and views have been created for BGP LS data.

### Jan-27-2015
**Release 0.8.0 is available.**   See [release-0.8.0](docs/release_notes/release-0.8.0.md) for more details.

> #### UPGRADE YOUR SCHEMA
> There have been schema changes, so please update your database.  Currently there isn't a migration
> script, so upgrading will require a drop of the current database. Routers will resend all data
> so all current/active info will come back, but the history will be lost. 
> 
> If you are concerned with the history being lost, please email me with the schema version you are 
> using and I can provide you the alter table syntax to migrate the tables without loss. 


### Nov-1-2014 
Added back BMPv1 support.  BMPv1 is supported best effort since it's missing the INIT, PEER UP, and TERM messages. Most things will work, but some of the DB views might need to be updated.  We'll update those as needed/requested. 

### Oct-29-2014
Added DNS PTR lookup for peers and routers.  Fixed minor issues and updated docs.  
Added [DB REST](docs/DBREST.md)


### Sep-10-2014
**Released version 0.7.1**   See [release-0.7.1](docs/release_notes/release-0.7.1.md) for more details.

> OpenBMP now fully supports draft-ietf-grow-bmp-07

**Upcoming Changes:**

  * Add BGP-LS support - IGP tables/views
  * OpenBMP UI is being revised using ODL


OpenBMP Flow
------------

![OpenBMP High Level Flow](docs/images/openbmp-highlevel-flow.png "OpenBMP High Level Flow")

1. BMP devices (e.g. routers) send BMP messages to a OpenBMP collector/daemon.   One OpenBMP daemon can handle many routers and bgp peers, but in a large network with transit links and full internet routing tables, multiple OpenBMP daemons is recommended.   Simply configure on the BMP device (router) which BMP server that should be used.  
2. Open Daylight (ODL) controller SQL plugin with SQL <-> Yang interfaces with the OpenBMP database.  ODL in this fashion provides an abstract view of all OpenBMP data.
3. Admins, Network Engineers, automated programs/scripts, etc. interact via ODL northbound interfaces to run various BMP analytics.
4. Admins, Network Engineers, automated programs/scripts, etc. can also go direct to the BMP database as needed. 

Supported Features
------------------
Below is a list of features supported today in OpenBMP.  Many more features are on the roadmap, including BGP-LS (draft-ietf-idr-ls-distribution).   See the **roadmap** for more details. 

Feature | Description
-------: | -----------
draft-ietf-grow-bmp-07| BMP Version 3
Database | Access to all collected data via standard ODBC/DB drivers
IPv4 | IPv4 Unicast routing table information
IPv6 | IPv6 Unicast routing table information
VPNv4 | L3VPN routing information
bgp-ls| draft-ietf-idr-ls-distribution
Extended Communities | Roughly all of them
Prefix Log| Tracking of withdraws and updates by prefix, including path attributes
Advanced Reporting| Built-in views for common reports, such as route tables, prefixes as paths, and route table history of changes

Use-Cases
---------
There are many reasons to use OpenBMP, but to highlight a few common ones:

* **Looking Glasses**  - IPv4, IPv6, and VPN4

* **Route Analytics** - Track convergence times, history of prefixes as they change over time, monitor and track BGP policy changes, etc...

* **Traffic Engineering Analytics**  - Adapt dynamically to change and know what is the best shift

* **BGP pre-policy What-Ifs** - Pre-policy routing information provides insight into all path attributes from various points in the network allowing nonintrusive what-if topology views for new policy validations

* **IGP Topology** - BGP-LS (link-state) provides the complete topology of the IGP (OSPF and/or IS-IS).  The IGP topology provides node, link, and prefix level information.  This includes all BGP next-hops.   It is now possible to do a BGP best path selection with IGP metric for **Adj-In-RIB** information.  It is also possible to monitor the IGP itself as it pertains to links, nodes, prefixes, and BGP.

* *many more*

Installation and Configuration
------------------------------
See the [docs/INSTALL.md](docs/INSTALL.md) documentation for detailed information on how to install and configure OpenBMP daemon and UI.

The installation documentation provides step by step instructions for how to install and configure OpenBMP, including the database.  

Instructions are for Ubuntu and CentOS/RHEL.   Other Linux distributions should work, but instructions might vary. 


Using Open Daylight
-------------------
See the [docs/ODL.md](docs/ODL.md) documentation for detailed information on how to use Open Daylight with OpenBMP.  

This includes details on how to setup ODL to use OpenBMP database(s).


Interfacing with the Database
-----------------------------
See the [docs/DATABASE.md](docs/DATABASE.md) documentation for the database schema and how to interact with it.    


Release Notes
----------
Check the release notes for changes by release.  



Road Map
--------
Below are a list of features/changes that are targeted in the next release:

### OpenBMP Daemon
* Inbound message caching to offload socket buffers
* New feature to allow saving raw BGP messages based on filters
* Add OpenBMP statistic counters to database
* Add TCP MD5SIG support for BMP sessions
* Add configuration option to restrict BMP devices to ones provisioned
* Add support for active BMP connections - OpenBMP makes connections to routers
* Add postgres DB support
* Implement RFC5424 logging with configuration options to fine tune

### BMP UI
Currently the UI is being developed.  Please contact **tievens@cisco.com** or **serpil@cisco.com** for more information. 

Building from Source
--------------------
See the [docs/BUILD.md](docs/BUILD.md) document for details on how to build OpenBMP from source.  Includes how to create DEB and RPM packages. 


