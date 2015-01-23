# <a id="configuring-icinga2-first-steps"></a> Configuring Icinga 2: First Steps

## <a id="icinga2-conf"></a> icinga2.conf

An example configuration file is installed for you in `/etc/icinga2/icinga2.conf`.

Here's a brief description of the example configuration:

    /**
     * Icinga 2 configuration file
     * - this is where you define settings for the Icinga application including
     * which hosts/services to check.
     *
     * For an overview of all available configuration options please refer
     * to the documentation that is distributed as part of Icinga 2.
     */

Icinga 2 supports [C/C++-style comments](13-language-reference.md#comments).

    /**
     * The constants.conf defines global constants.
     */
    include "constants.conf"

The `include` directive can be used to include other files.

    /**
     * The zones.conf defines zones for a cluster setup.
     * Not required for single instance setups.
     */
     include "zones.conf"

    /**
     * The Icinga Template Library (ITL) provides a number of useful templates
     * and command definitions.
     * Common monitoring plugin command definitions are included separately.
     */
    include <itl>
    include <plugins>

    /**
     * The features-available directory contains a number of configuration
     * files for features which can be enabled and disabled using the
     * icinga2 feature enable / icinga2 feature disable CLI commands.
     * These commands work by creating and removing symbolic links in
     * the features-enabled directory.
     */
    include "features-enabled/*.conf"

This `include` directive takes care of including the configuration files for all
the features which have been enabled with `icinga2 feature enable`. See
[Enabling/Disabling Features](6-cli-commands.md#features) for more details.

    /**
     * The repository.d directory contains all configuration objects
     * managed by the 'icinga2 repository' CLI commands.
     */
    include_recursive "repository.d"

This `include_recursive` directive is used for discovery of services on remote clients
and their generated configuration described in
[this chapter](5-monitoring-remote-systems.md#icinga2-remote-monitoring-master-discovery-generate-config).


    /**
     * Although in theory you could define all your objects in this file
     * the preferred way is to create separate directories and files in the conf.d
     * directory. Each of these files must have the file extension ".conf".
     */
    include_recursive "conf.d"

You can put your own configuration files in the [conf.d](4-configuring-icinga-2.md#conf-d) directory. This
directive makes sure that all of your own configuration files are included.

> **Tip**
>
> The example configuration is shipped in this directory too. You can either
> remove it entirely, or adapt the existing configuration structure with your
> own object configuration.

## <a id="constants-conf"></a> constants.conf

The `constants.conf` configuration file can be used to define global constants.

By default, you need to make sure to set these constants:

* The `PluginDir` constant must be pointed to your [check plugins](2-getting-started.md#setting-up-check-plugins) path.
This constant is required by the shipped
[plugin check command configuration](16-icinga-template-library.md#plugin-check-commands).
* The `NodeName` constant defines your local node name. Should be set to FQDN which is the default
if not set. This constant is required for local host configuration, monitoring remote clients and
cluster setup.

Example:

    /* The directory which contains the plugins from the Monitoring Plugins project. */
    const PluginDir = "/usr/lib64/nagios/plugins"


    /* The directory which contains the Manubulon plugins.
     * Check the documentation, chapter "SNMP Manubulon Plugin Check Commands", for details.
     */
    const ManubulonPluginDir = "/usr/lib64/nagios/plugins"

    /* Our local instance name. By default this is the server's hostname as returned by `hostname --fqdn`.
     * This should be the common name from the API certificate.
     */
    //const NodeName = "localhost"

    /* Our local zone name. */
    const ZoneName = NodeName

    /* Secret key for remote node tickets */
    const TicketSalt = ""

The `ZoneName` and `TicketSalt` constants are required for remote client
and distributed setups only.

## <a id="conf-d"></a> The conf.d Directory

This directory contains example configuration which should help you get started
with monitoring the local host and its services. It is included in the
[icinga2.conf](4-configuring-icinga-2.md#icinga2-conf) configuration file by default.

It can be used as reference example for your own configuration strategy.
Just keep in mind to include the main directories in the
[icinga2.conf](4-configuring-icinga-2.md#icinga2-conf) file.

You are certainly not bound to it. Remove it, if you prefer your own
way of deploying Icinga 2 configuration.

Further details on configuration best practice and how to build your
own strategy is described in [this chapter](3-monitoring-basics.md#configuration-best-practice).

Available configuration files shipped by default:

* [hosts.conf](4-configuring-icinga-2.md#hosts-conf)
* [services.conf](4-configuring-icinga-2.md#services-conf)
* [users.conf](4-configuring-icinga-2.md#users-conf)
* [notifications.conf](4-configuring-icinga-2.md#notifications-conf)
* [commands.conf](4-configuring-icinga-2.md#commands-conf)
* [groups.conf](4-configuring-icinga-2.md#groups-conf)
* [templates.conf](4-configuring-icinga-2.md#templates-conf)
* [downtimes.conf](4-configuring-icinga-2.md#downtimes-conf)
* [timeperiods.conf](4-configuring-icinga-2.md#timeperiods-conf)
* [satellite.conf](4-configuring-icinga-2.md#satellite-conf)

### <a id="hosts-conf"></a> hosts.conf

The `hosts.conf` file contains an example host based on your
`NodeName` setting in [constants.conf](4-configuring-icinga-2.md#constants-conf). You
can use global constants for your object names instead of string
values.

The `import` keyword is used to import the `generic-host` template which
takes care of setting up the host check command to `hostalive`. If you
require a different check command, you can override it in the object definition.

The `vars` attribute can be used to define custom attributes which are available
for check and notification commands. Most of the [Plugin Check Commands](16-icinga-template-library.md#plugin-check-commands)
in the Icinga Template Library require an `address` attribute.

The custom attribute `os` is evaluated by the `linux-servers` group in
[groups.conf](4-configuring-icinga-2.md#groups-conf) making the local host a member.

The example host will show you how to

* define http vhost attributes for the `http` service apply rule defined
in [services.conf](4-configuring-icinga-2.md#services-conf).
* define disks (all, specific `/`) and their attributes for the `disk`
service apply rule defined in [services.conf](4-configuring-icinga-2.md#services-conf).
* define notification types (`mail`) and set the groups attribute. This
will be used by notification apply rules in [notifications.conf](notifications-conf).

If you've installed [Icinga Web 2](2-getting-started.md#setting-up-icingaweb2) you can
uncomment the http vhost attributes and relaod Icinga 2. The apply
rules in [services.conf](4-configuring-icinga-2.md#services-conf) will automatically
generate a new service checking the `/icingaweb2` URI using the `http`
check.

    /*
     * Host definitions with object attributes
     * used for apply rules for Service, Notification,
     * Dependency and ScheduledDowntime objects.
     *
     * Tip: Use `icinga2 object list --type Host` to
     * list all host objects after running
     * configuration validation (`icinga2 daemon -C`).
     */

    /*
     * This is an example host based on your
     * local host's FQDN. Specify the NodeName
     * constant in `constants.conf` or use your
     * own description, e.g. "db-host-1".
     */

    object Host NodeName {
      /* Import the default host template defined in `templates.conf`. */
      import "generic-host"

      /* Specify the address attributes for checks e.g. `ssh` or `http`. */
      address = "127.0.0.1"
      address6 = "::1"

      /* Set custom attribute `os` for hostgroup assignment in `groups.conf`. */
      vars.os = "Linux"

      /* Define http vhost attributes for service apply rules in `services.conf`. */
      vars.http_vhosts["http"] = {
        http_uri = "/"
      }
      /* Uncomment if you've sucessfully installed Icinga Web 2. */
      //vars.http_vhosts["Icinga Web 2"] = {
      //  http_uri = "/icingaweb2"
      //}

      /* Define disks and attributes for service apply rules in `services.conf`. */
      vars.disks["disk"] = {
        /* No parameters. */
      }
      vars.disks["disk /"] = {
        disk_partitions = "/"
      }

      /* Define notification mail attributes for notification apply rules in `notifications.conf`. */
      vars.notification["mail"] = {
        /* The UserGroup `icingaadmins` is defined in `users.conf`. */
        groups = [ "icingaadmins" ]
      }
    }

This is only the host object definition. Now we'll need to make sure that this
host and your additional hosts are getting [services](4-configuring-icinga-2.md#services-conf) applied.

> **Tip**
>
> If you don't understand all the attributes and how to use [apply rules](13-language-reference.md#apply)
> don't worry - the [monitoring basics](3-monitoring-basics.md#monitoring-basics) chapter will explain
> that in detail.

### <a id="services-conf"></a> services.conf

These service [apply rules](13-language-reference.md#apply) will show you how to monitor
the local host, but also allow you to re-use or modify them for
your own requirements.

You should define all your service apply rules in `services.conf`
or any other central location keeping them organized.

By default, the local host will be monitored by the following services

Service(s)                                  | Applied on host(s)
--------------------------------------------|------------------------
`load`, `procs`, `swap`, `users`, `icinga`  | The `NodeName` host only
`ping4`, `ping6`                            | All hosts with `address` resp. `address6` attribute
`ssh`                                       | All hosts with `address` and `vars.os` set to `Linux`
`http`, optional: `Icinga Web 2`            | All hosts with custom attribute `http_vhosts` defined as dictionary
`disk`, `disk /`                            | All hosts with custom attribute `disks` defined as dictionary

The Debian packages also ship an additional `apt` service check applied to the local host.

The command object `icinga` for the embedded health check is provided by the
[Icinga Template Library (ITL)](16-icinga-template-library.md#icinga-template-library) while `http_ip`, `ssh`, `load`, `processes`,
`users` and `disk` are all provided by the [Plugin Check Commands](16-icinga-template-library.md#plugin-check-commands)
which we enabled earlier by including the `itl` and `plugins` configuration file.


Example `load` service apply rule:

    apply Service "load" {
      import "generic-service"

      check_command = "load"

      /* Used by the ScheduledDowntime apply rule in `downtimes.conf`. */
      vars.backup_downtime = "02:00-03:00"

      assign where host.name == NodeName
    }

The `apply` keyword can be used to create new objects which are associated with
another group of objects. You can `import` existing templates, define (custom)
attributes.

The custom attribe `backup_downtime` is defined to a specific timerange string.
This variable value will be used for applying a `ScheduledDowntime` object to
these services in [downtimes.conf](4-configuring-icinga-2.md#downtimes-conf).

In this example the `assign where` condition is a boolean expression which is
evaluated for all objects of type `Host` and a new service with name "load"
is created for each matching host. [Expression operators](13-language-reference.md#expression-operators)
may be used in `assign where` conditions.

Multiple `assign where` condition can be combined with `AND` using the `&&` operator
as shown in the `ssh` example:

    apply Service "ssh" {
      import "generic-service"

      check_command = "ssh"

      assign where host.address && host.vars.os == "Linux"
    }

In this example, the service `ssh` is applied to all hosts having the `address`
attribute defined `AND` having the custom attribute `os` set to the string
`Linux`.
You can modify this condition to match multiple expressions by combinding `AND`
and `OR` using `&&` and `||` [operators](13-language-reference.md#expression-operators), for example
`assign where host.address && (vars.os == "Linux" || vars.os == "Windows")`.


A more advanced example is shown by the `http` and `disk` service apply
rules. While one `apply` rule for `ssh` will only create a service for matching
hosts, you can go one step further: Generate apply rules based on array items
or dictionary key-value pairs.

The idea is simple: Your host in [hosts.conf](4-configuring-icinga-2.md#hosts-conf) defines the
`disks` dictionary as custom attribute in `vars`.

Remember the example from [hosts.conf](4-configuring-icinga-2.md#hosts-conf):

    ...

      /* Define disks and attributes for service apply rules in `services.conf`. */
      vars.disks["disk"] = {
        /* No parameters. */
      }
      vars.disks["disk /"] = {
        disk_partition = "/"
      }
    ...


This dictionary contains multiple service names we want to monitor. `disk`
should just check all available disks, while `disk /` will pass an additional
parameter `disk_partition` to the check command.

You'll recognize that the naming is important - that's the very same name
as it is passed from a service to a check command argument. Read about services
and passing check commands in [this chapter](3-monitoring-basics.md#command-passing-parameters).

Using `apply Service for` omits the service name, it will take the key stored in
the `disk` variable in `key => config` as new service object name.

The `for` keyword expects a loop definition, for example `key => value in dictionary`
as known from Perl and other scripting languages.

Once defined like this, the `apply` rule defined below will do the following:

* only match hosts with `host.vars.disks` defined through the `assign where` condition
* loop through all entries in the `host.vars.disks` dictionary. That's `disk` and `disk /` as keys.
* call `apply` on each, and set the service object name from the provided key
* inside apply, the `generic-service` template is imported
* defining the [disk](16-icinga-template-library.md#plugin-check-command-disk) check command requiring command arguments like `disk_partition`
* adding the `config` dictionary items to `vars`. Simply said, there's now `vars.disk_partition` defined for the
generated service

Configuration example:

    apply Service for (disk => config in host.vars.disks) {
      import "generic-service"

      check_command = "disk"

      vars += config

      assign where host.vars.disks
    }

A similar example is used for the `http` services. That way you can make your
host the information provider for all apply rules. Define them once, and only
manage your hosts.

Look into [notifications.conf](4-configuring-icinga-2.md#notifications-conf) how this technique is used
for applying notifications to hosts and services using their type and user
attributes.

Don't forget to install the [check plugins](2-getting-started.md#setting-up-check-plugins) required by
the hosts and services and their check commands.

Further details on the monitoring configuration can be found in the
[monitoring basics](3-monitoring-basics.md#monitoring-basics) chapter.

### <a id="users-conf"></a> users.conf

Defines the `icingaadmin` User and the `icingaadmins` UserGroup. The latter is used in
[hosts.conf](4-configuring-icinga-2.md#hosts-conf) for defining a custom host attribute later used in
[notifications.conf](4-configuring-icinga-2.md#notifications-conf) for notification apply rules.

    object User "icingaadmin" {
      import "generic-user"

      display_name = "Icinga 2 Admin"
      groups = [ "icingaadmins" ]

      email = "icinga@localhost"
    }

    object UserGroup "icingaadmins" {
      display_name = "Icinga 2 Admin Group"
    }


### <a id="notifications-conf"></a> notifications.conf

Notifications for check alerts are an integral part or your
Icinga 2 monitoring stack.

The shipped example defines two notification apply rules for hosts and services.
Both `apply` rules match on the same condition: They are only applied if the
nested dictionary attribute `notification.mail` is set.

Please note that the `to` keyword is important in [notification apply rules](3-monitoring-basics.md#using-apply-notifications)
defining whether these notifications are applies to hosts or services.
The `import` keyword imports the specific mail templates defined in [templates.conf](4-configuring-icinga-2.md#templates-conf).

The `interval` attribute is not explicitly set - it [defaults to 30 minutes](15-object-types.md#objecttype-notification).

By setting the `user_groups` to the value provided by the
respective [host.vars.notification.mail](4-configuring-icinga-2.md#hosts-conf) attribute we'll
implicitely use the`icingaadmins` UserGroup defined in [users.conf](4-configuring-icinga-2.md#users-conf).

    apply Notification "mail-icingaadmin" to Host {
      import "mail-host-notification"

      user_groups = host.vars.notification.mail.groups

      assign where host.vars.notification.mail
    }

    apply Notification "mail-icingaadmin" to Service {
      import "mail-service-notification"

      user_groups = host.vars.notification.mail.groups

      assign where host.vars.notification.mail
    }

More details on defining notifications and their additional attributes such as
filters can be read in [this chapter](3-monitoring-basics.md#notifications).

### <a id="commands-conf"></a> commands.conf

This is the place where your own command configuration can be defined. By default
only the notification commands used by the notification templates defined in [templates.conf](4-configuring-icinga-2.md#templates-conf).

> **Tip**
>
> Icinga 2 ships the most common command definitions already in the
> [Plugin Check Commands](16-icinga-template-library.md#plugin-check-commands) definitions. More details on
> that topic in the [troubleshooting section](10-troubleshooting.md#check-command-definitions).

You can freely customize these notification commands, and adapt them for your needs.
Read more on that topic [here](3-monitoring-basics.md#notification-commands).

### <a id="groups-conf"></a> groups.conf

The example host defined in [hosts.conf](hosts-conf) already has the
custom attribute `os` set to `Linux` and is therefore automatically
a member of the host group `linux-servers`.

This is done by using the [group assign](13-language-reference.md#group-assign) expressions similar
to previously seen [apply rules](3-monitoring-basics.md#using-apply).

    object HostGroup "linux-servers" {
      display_name = "Linux Servers"

      assign where host.vars.os == "Linux"
    }

    object HostGroup "windows-servers" {
      display_name = "Windows Servers"

      assign where host.vars.os == "Windows"
    }

Service groups can be grouped together by similar pattern matches.
The [match() function](13-language-reference.md#function-calls) expects a wildcard match string
and the attribute string to match with.

    object ServiceGroup "ping" {
      display_name = "Ping Checks"

      assign where match("ping*", service.name)
    }

    object ServiceGroup "http" {
      display_name = "HTTP Checks"

      assign where match("http*", service.check_command)
    }

    object ServiceGroup "disk" {
      display_name = "Disk Checks"

      assign where match("disk*", service.check_command)
    }


### <a id="templates-conf"></a> templates.conf

All shipped example configuration objects use generic global templates by
default. Be it setting a default `check_command` attribute in the `generic-host`
templates for your hosts defined in [hosts.conf](4-configuring-icinga-2.md#hosts-conf), or defining
the default `states` and `types` filters for notification apply rules defined in
[notifications.conf](4-configuring-icinga-2.md#notifications-conf).


    template Host "generic-host" {
      max_check_attempts = 5
      check_interval = 1m
      retry_interval = 30s

      check_command = "hostalive"
    }

    template Service "generic-service" {
      max_check_attempts = 3
      check_interval = 1m
      retry_interval = 30s
    }

The `hostalive` check command is shipped with Icinga 2 in the
[Plugin Check Commands](16-icinga-template-library.md#plugin-check-commands).


    template Notification "mail-host-notification" {
      command = "mail-host-notification"

      states = [ Up, Down ]
      types = [ Problem, Acknowledgement, Recovery, Custom,
                FlappingStart, FlappingEnd,
                DowntimeStart, DowntimeEnd, DowntimeRemoved ]

      period = "24x7"
    }

    template Notification "mail-service-notification" {
      command = "mail-service-notification"

      states = [ OK, Warning, Critical, Unknown ]
      types = [ Problem, Acknowledgement, Recovery, Custom,
                FlappingStart, FlappingEnd,
                DowntimeStart, DowntimeEnd, DowntimeRemoved ]

      period = "24x7"
    }

More details on `Notification` object attributes can be found [here](15-object-types.md#objecttype-notification).


### <a id="downtimes-conf"></a> downtimes.conf

The `load` service apply rule defined in [services.conf](4-configuring-icinga-2.md#services-conf) defines
the `backup_downtime` custom attribute.

The [ScheduledDowntime](15-object-types.md#objecttype-scheduleddowntime) apply rule uses this attribute
to define the default value for the time ranges required for recurring downtime slots.

    apply ScheduledDowntime "backup-downtime" to Service {
      author = "icingaadmin"
      comment = "Scheduled downtime for backup"

      ranges = {
        monday = service.vars.backup_downtime
        tuesday = service.vars.backup_downtime
        wednesday = service.vars.backup_downtime
        thursday = service.vars.backup_downtime
        friday = service.vars.backup_downtime
        saturday = service.vars.backup_downtime
        sunday = service.vars.backup_downtime
      }

      assign where service.vars.backup_downtime != ""
    }


### <a id="timeperiods-conf"></a> timeperiods.conf

This file ships the default timeperiod definitions for `24x7`, `9to5`
and `never`. Timeperiod objects are referenced by `*period`
objects such as hosts, services or notifications.


### <a id="satellite-conf"></a> satellite.conf

Ships default templates and dependencies for [monitoring remote clients](5-monitoring-remote-systems.md#icinga2-remote-client-monitoring)
using service discovery and [config generation](5-monitoring-remote-systems.md#icinga2-remote-monitoring-master-discovery-generate-config)
on the master. Can be ignored/removed on setups not using this features.


Further details on the monitoring configuration can be found in the
[monitoring basics](3-monitoring-basics.md#monitoring-basics) chapter.