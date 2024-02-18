# meta-mercury

## Hola mundo

aaaaaa

- asdasd
- adsasd
- asdasdassad

```
print("asda")
```

## Download Mercury
- [Download Mercury](https://github.alm.europe.cloudcenter.corp/pages/cib-mercury-quants/meta-mercury/mercury/)
- [Download Mercury (admin mode)](https://github.alm.europe.cloudcenter.corp/pages/cib-mercury-quants/meta-mercury/mercury/?token=mercury)

## What is meta-mercury
meta-mercury is a container of projects to contain all Mercury depends in one. This makes it easier to compile and manage.

## Request Git authorization
Send your username and publish ssh key to `dl_quantslinearratesinflationtools@gruposantander.com` requesting access to git. You can found this file in `%USERPROFILE%/.ssh/id_rsa.pub` (C:\Users\nXXXXXX\\.ssh\id_rsa.pub).

Node: Please send only public (.pub), not send your private key!!.

## Git configuration
First time, is mandatory configure Git, for avoid problems with merges and checkouts. Open a cmd typing `cmd` and paste:
```
git config --global http.sslVerify true
git config --global http.sslBackend schannel
git config --global merge.commit no
git config --global merge.ff no
git config --global pull.ff yes
git config --global lfs.https://github.alm.europe.cloudcenter.corp/cib-mercury-quants/meta-mercury.git/info/lfs.locksverify true
git config --global lfs.https://github.alm.europe.cloudcenter.corp/cib-mercury-quants/externallibs.git/info/lfs.locksverify true
git config --global lfs.https://github.alm.europe.cloudcenter.corp/cib-mercury-quants/excel.git/info/lfs.locksverify true
git config --global lfs.https://github.alm.europe.cloudcenter.corp/cib-mercury-quants/test.git/info/lfs.locksverify true
```
Other recommended config, is your name and email:
```
git config --global user.name "Your Name"
git config --global user.email "youremail@yourdomain.com"
```

## From Zero to Mercury
Prepare a empty folder and change to it. Open a cmd typing `cmd` and paste:
```
cd C:\projects_folder
mkdir mercury_git
cd mercury_git
```
Download and Build, typing (change to empty directory mercury_git):
```
git lfs clone --recurse-submodules -j8 git@github.alm.europe.cloudcenter.corp:cib-mercury-quants/meta-mercury.git .
```

Before to build, check C:\Mercury and C:\Projects are available (it can exists if are symbolic links):
```
cd C:\
move C:\Mercury C:\Mercury_bak
move C:\Projects C:\Projects_bak
```
Any finally build, to compile Mercury:
```
build64
```
Can compile in 32 bits with build32.
This take time the first time, but incremental updates are fast.

## Update and Build Mercury
No need start from zero, if you have changes, can update meta-mercury and keeping local changes. Open a cmd typing `cmd` and paste:
```
update
build64
```

## All in one (oneliner)
Execute it in a empty folder:
```
git lfs clone --recurse-submodules -j8 git@github.alm.europe.cloudcenter.corp:cib-mercury-quants/meta-mercury.git . && build64 --version True
```
In next executions:
```
update && build64 --version True
```
The parameter ``--version True` means generate a Mercury installer using Inno Setup.

## Testing
Mercury tests. Type:
```
tests64
```
Or execute ``tests32` if you do previously `build32`.
We have also jenkins tests, but based in Python3 (C:\Miniconda3). Type:
```
tests_jenktools
```

# Advanced usage

## Update to old tag/commit and Build Mercury
For sync to old tag,  open a cmd typing `cmd` and paste:
```
update (optional but recommended)
git checkout -f <BRANCH/TAG/COMMIT>
sync-tag-commit
build64
```

## Switch to release/hotfix branch
For switch to branch (or to master), type:
```
update release_2023Jul28
```
where `release_2023Jul28` is branch name in this example.

## Up-to-date release branch with master
After switch to release/hotfix branch, we can type
```
merge-from-master
```
