# Git guide

## Git rules proposals

  -	No rewrite history in shared branchs. Disable fast-forward and not use rebase in shared branchs.
  -	We develop in “master”, is forbidden direct commit in “production”. Branch “production” only can change receiving merges from “release” branchs or “hotfix” branch.
  -	Advance in flows require use Jenkins for check you are in safe point.
  -	Follow the flows schemas and identify what are you doing.
  
## Git commands

Clone repository:
```
git clone git@github.alm.europe.cloudcenter.corp:cib-mercury-quants/jenktools.git
```
Clone repository with LFS support and submodules:
```
git lfs clone --recurse-submodules -j8 git@github.alm.europe.cloudcenter.corp:cib-mercury-quants/meta-mercury.git
```

Or starting from git empty repository:
```
git init
git remote add origin git@github.alm.europe.cloudcenter.corp:cib-mercury-quants/meta-mercury.git
```
  
Create a new branch (and switch to it):
```
git switch -c new_branch
git checkout -b new_branch (equivalent but deprecated way)
```
  
Add new file:
```
git add filename
```
  
Add all files recursively:
```
git add -A
```
  
Remove file in git (and in local):
```
git rm filename
```
  
Check git state of repository:
```
git status
```
  
See local differences:
```
git diff
```
  
Commit changes in local branch:
```
git commit -m “message commit”
```
  
Commit changes (with auto-staging, auto-add files, very common):
```
git commit -a -m “message commit”
```
  
Sync (Push local commits to remote branch):
```
git push
```
 
Update local branch from remote:
```
git pull
```
  
Upload new local branch (if you use autoSetupRemote=false in config, this is default value):
```
git push -u origin new_branch
```
  
Pull changes in branches (sometimes need to be explicit):
```
git pull origin new_branch
```
  
View log history in graph:
```
git log --oneline –graph
```
  
View log history in graph (limit 10 last changesets):
```
git log --oneline –graph -10
```
  
Ignore local changes and commits and reset “hard” to remote.
```
git reset --hard origin/HEAD
```
  
Print current revision (git hash):
```
git rev-parse HEAD
```
  
Print current branch name:
```
git rev-parse --abbrev-ref HEAD
```
  
Merge from master to “branch_name” (remember disable fast forward):
```
git switch branch_name
git merge master
```
  
Merge from “branch_name” to master:
```
  -	Merge highly recommended (releases and hotfixes, disable fast forward):
  -		git merge branch_name
  -	Merge removing signals from your branch (only for feature branchs):
  -		git merge --ff --squash branch_name && git commit -m “….”
```

See local branches:
```
git branch
```
  
See remote branches:
```
git branch -r
```
  
See local and remote branches:
```
git branch -a
```
 
Selective merge changeset:
```
git switch branch_name
git cherry-pick 1cbb3e6
```
  
Clean ignored files (unversioned and files in .gitignore):
```
git clean -xdf
```
  
Delete local branch:
```
git branch -D branch_name
```
  
Delete remote branch:
```
git push origin --delete branch_name
```
  
Conflicts can happen meanwhile a merge or pull (pull is same that “fetch + merge”). After a conflict, can use:
```
  -	git merge --continue (after solving conflicts and continue merge)
  -	or git merge –abort for revert to previous state before to do merge.
  -	Remember: always have all committed before do a merge, sometimes git merge –abort can’t recover changes.
```

Add a tag a specific commit:
```
  -	git switch -c temporalbranch cc27887 or git checkout -f cc27887
  -	git tag -a release/2023.02.23 -m "[Release] 2023-02-23"
```
  
List all tags available:
```
git tag
```
  
Checkout a tag:
```
git switch --detach release/2023.02.23 (because is a HEAD detached commit)
    or simpler: git checkout release/2023.02.23
```
  
Remove a local tag:
```
git tag -d release/2023.02.23
```
  
Remove a remote tag, is same that remove a remote branch:
```
git push origin --delete tag_name
```
  
Upload local tag to remote:
```
git push origin release/2023.02.23
```
  
List remote tags and see commits:
```
git ls-remote --tags origin
```
  
Download all remote tags:
```
git fetch --all –tags
```
 
Print last tag available:
```
git describe
```
  
Create a patch between tags:
```
  -	cd %base_dir%
  -	git diff tag1 tag2 > incremental_version.patch
```
Apply a patch:
```
  -	cd %base_dir% (apply patch in equivalent dir to creation patch)
  -	git apply incremental_version.patch
```
Remove local changes (but keep in other space):
```
  -	git stash
```
  
List changes saved with stash:
```
 git stash list
```
  
Recover changes in stash:
```
git stash apply
git stash drop (or in one step directly “git stash pop”, but be careful with conflicts)
```

Get annotation tag:
```
git tag -l -n999 --format="%(subject)" $tagname
```

## Hack commands

Modify tag anotation (be careful)
```
git tag startrel-2023Sep01.1 -f -m "{\"repo\":\".\",\"current_branch\":\"release_2023Sep01.1\",\"origin_branch\":\"master\",\"branch\":\"master\",\"tag_start_point\":\"startrel-2023Sep01.1\",\"branch_solved\":\"release_2023Sep01.1\",\"creation_date\":\"2023Sep01.1\"}"
git push -f origin startrel-2023Sep01.1 
```
