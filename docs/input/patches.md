# Patching dynamic code
This is a system for apply patches in user version. This only works with no freeze code, like site-packages scripts.

# How create a patch
Using "patches-create" in c:\Mercury\patches-create.cmd. Examples:
> NOTE: In these examples, the working directory is C:\Mercury

## Using "patches-create" and git
- Using patches-creates without positional parameters, means create a patch ``git diff HEAD``. (This is diff of local changes (including staged and not staged) vs last commit ``HEAD``). Can compare since other base commit or tag vs your local changes, with parameter ``--base``.
  - ```
    patches-create --git
    ```
    Other example with custom name in private channel.
    ```
    patches-create --git --channel change1 --name change1
    ```
  - We have more important parameters for patch:
    - ``name``: name of patch. Must be unique in your channel.
    - ``description``: description of patch.
    - ``channel``: channel name where your upload the patch. The channel recommended are ``DEV`` for internal use and ``PRO`` is default channel for users.
    - ``applicability``: is a patterns list that reply to the answer. who should install this patch?
- After that, you will upload the patch ``name`` to ``channel``.

## How create a patch without git
- Using patches-create with positional parameters, which is enumerate explicitly files to make the patch. 
  - ```
    patches-create file1 file2 file3 ...
    ```
  - Example:
    - ```
      patches-create site-packages\Patches\apply.py patches-apply.cmd patches-create.cmd Patches\__init__.py site-packages\Patches\test_server.py
      ```
  - In the same way as before, the program will interactively ask us for ```name```, ``description`` and ``channel``.

## Avoid interactive prompts
You can use explicit parameters: ``--name``, ``--description``, ``--channel`` and ``--applicability``.
> **_NOTE:_**  ``--applicability or -a`` is a list of patterns and is appendable. Example: ``-a versionmcy -a hostname1 -a hostname -a user``

Example with all explicit parameters (no interaction): ``
patches-create site-packages\Patches\diff.py site-packages\Patches\apply.py patches-apply.cmd patches-create.cmd Patches\__init__.py site-packages\Patches\test_server.py Patches\config.py site-packages\Patches\test_server.py --name updater_2.4 --description "test" --channel PRO -a *Nov2022
``.
If a patch have same ``name`` and ``channel`` will fail upload, but can use ``--replace`` to upload again, but avoid this practice because maybe the already applied your old patch.

# What values should have in ``applicability``
We can type three type of values:
- ``version``: content from version.txt or version64.txt. You can use patterns. Example: ``*Nov2022``
- ``hostname``: machine hostname, for apply patches only applicable in that machine. Example: ``CIBBOA1N332211R``
- ``username``: this is useful for security reasons, are personal patches, only applied to one person (patch creator probably). Example: n443322

# How apply patches (update Mercury)
Using "patches-apply" in c:\Mercury\patches-apply.cmd. We have multiples use cases.

## Update all patches that are applicable to me
- This is default case for patches-apply.
  - ```
    patches-apply
    ```
  - This apply all patches in correct order and applicable to me. If already patch was applied previously, this will not install.

## Apply with parameters
### channel
Selecting source of paths with ``--channel``:
Applied all patches from channel "DEV":
  - ```
    patches-apply --channel DEV
    ```

### force-update
If a patch is already applied, don't will install anymore. You can change this behavior with parameter ``--force-update``.
  - ```
    patches-apply --force-update
    ```
    This apply all patches applicable to me in correct order.

### force-add
A patch for security reasons, only modify existing files. Patches do not create files or delete them. You can change it with ``--force-add``. Since you already applied it, you will also need to combine it with `--force-update`:
  - ```
    patches-apply --force-add --force-update
    ```
### mercury-path
You can apply the patches by changing the target directory. Example:
``
patches-apply --channel DEV --mercury-path C:\dev\Mercury --force-update --force-add
``
This applied the patch in a checkout of Mercury in C:\Mercury, combined with other parameters already explained.

### version
You can apply only one version patch using ``--version name`` or even use fnmatch patterns like ``--version *Abr2023*``.
By default is: ``--version *``. That means that all patches are applicable to me.

We can use an impossible pattern for list available paths:
```
patches-apply --version QWERTYASDF
```
Other example to force installation of one patch (and add installation info with ``-v``):
```
patches-apply --version *updater_2.6 --force-add --force-update -v
```

# How start a Windfall Server
Modify ``Patches\config.py`` and comment next lines:
- default_network_drive = 'W:\\'
- default_windfall_host = None
- default_windfall_port = None

And add/change it for your values. Example:
- default_network_drive = 'W:\\'             # your share folder
- default_windfall_host = 'CIBBOA1N424613R'  # your hostname
- default_windfall_port = 50030

This modified config, is need for "creators" of patches and for "consumers" of patches, that is to say Server and Clients need same config for connect to same host and port.
We have in roadmap one config depends your ``location``: ``MAD``, ``UK`` etc. ...

For fast server testing execute:
```
python site-packages/Patches/test_server.py
```

For coming this to production, need execute it like a service using ``StorageService/Windfall_FileServer_WinService.py``.

# patches-create --help, -h
```
usage: McyScriptLoader.py [-h] [-b BASE] [--name NAME]
                          [--description DESCRIPTION] [-c CHANNEL]
                          [--mercury-path MERCURY_PATH]
                          [--remote-path REMOTE_PATH] [--user USER]
                          [--replace] [--log LOG] [-v]
                          [-a [applicability [applicability ...]]]
                          patch [patch ...]

Create a patch and upload to Server

positional arguments:
  patch                 List files to patch.

optional arguments:
  -h, --help            show this help message and exit
  -b BASE, --base BASE  tag or commit hash of reference. Default is HEAD (last
                        commit). (Default: HEAD)
  --name NAME           name of patch. (Default: None)
  --description DESCRIPTION
                        Description for this patch. (Default: None)
  -c CHANNEL, --channel CHANNEL
                        channel through which you are sending the patch.
                        Examples: DEV/PRO... (Default: None)
  --mercury-path MERCURY_PATH
                        Mercury Path (default: C:\Mercury)
  --remote-path REMOTE_PATH
                        Share Folder Path (default: W:\)
  --user USER           Folder to use in Windfall Remote Server (default: PATCHES)
  --replace             Force to replace patches. (Default: False)
  --log LOG             Path where to save the log (default: C:\Mercury\log)
  -v, --verbose         Print copied files by patch. Update time can increase.
  (default: False)
  -a [applicability [applicability ...]], --applicability [applicability [applicability ...]]
                        Who does this patch apply to? Appendable.
                        [VERSION_MCY,MACHINE,USER] (default: None)
```
# patches-apply --help, -h
```
usage: McyScriptLoader.py [-h] [--version VERSION] [--host HOST] [--port PORT]
                          [-c CHANNEL] [--mercury-path MERCURY_PATH]
                          [--user USER] [--force-add] [--force-update]
                          [--no-close] [--log LOG] [-v]

Download and apply a patch from Server.

optional arguments:
  -h, --help            show this help message and exit
  --version VERSION     Version to Update. (Is a pattern of fnmatch. Example: *07*). (Default: *)
  --host HOST           Windfall Remote Server hostname (default: auto)
  --port PORT           Port of Server Storage (default: auto)
  -c CHANNEL, --channel CHANNEL
                        channel through which you are sending the patch.
                        Examples: DEV/PRO... (default: PRO)
  --mercury-path MERCURY_PATH
                        Mercury Path (default: C:\Mercury)
  --user USER           Folder to use in Windfall Remote Server                       (default: PATCHES)
  --force-add           Force to install even target files don't exists.
                        (default: False)
  --force-update        Force updates even if patch was already applied.
                        (default: False)
  --no-close            No force close excel. (Default: False)
  --log LOG             Path where to save the log (default: C:\Mercury\log)
  -v, --verbose         Print copied files by patch. Update time can increase.
                        (default: False)
```

# Copy changes from one workspace to other
Imagine need copy changes from C:\Mercury to C:\dev\Mercury.
First, we create a patch in the source workspace:
```
cd c:\Metcury
patches-copy
```
It get git changes, but if we dont have git (because are in cvs or other reason), then must be explicit in list files:
```
cd c:\Metcury
patches-copy file1 file2 file3 file4 ...
```
We apply the patch in our destination:
```
cd c:\dev\Mercury
patches-paste
```
