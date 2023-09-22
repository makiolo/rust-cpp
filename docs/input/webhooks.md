# Remote control of Jenkins (via webhook)

This examples using curl via command line [https://curl.se/windows/], but port it to javascript or python is direct.

# Webhook generic parameters (common to all jobs)
- name: Exact Job Name or special value '*' for execute all jobs.
- node: machine to use. (prefered to use if flexible=true)
- flexible: not let to scheduler select machine.
- cleanup: remove workspace before to start.
- refresh: it update Jenkinsfile, and only that. It do a early exit aborted.
- send_email: send email or not.
- notify: TODO: send changes of state to Teams, or not.

## Refresh Jenkinsfiles

Jenkins have a problem refresing Jenkinsfile, one change in parameters, need two executions to have effective effect.
For avoid it, we can force refresh all Jenkinsfiles with this webhook:
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '*', 'refresh':true, 'node':'principal'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Refresh Jenkinsfiles and clean workspace (in concrete machine)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '*', 'refresh':true, 'cleanup':true, 'node':'principal', 'flexible': 'false'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Create new release (from master)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '1.create_release', 'node':'slave1'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```
## Create new release (from commit hash)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '1.create_release', 'node':'slave1', 'branch': '9d12676b3707152c2041b60aa96ab5cbcefddb3c'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish release (from last created release branch) (build 32 and 64 in sequence)
Use cleanup=false for incremental releases.
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_release', 'node':'slave1'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish release (from last created release branch) (build 32 and 64 in parallel)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_release', 'node':'slave1', 'parallel_build': 'true'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish release (from last created release branch) (only build 32)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_release', 'node':'slave1', 'generate_64': 'false'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish release (from last created release branch) (only build 64)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_release', 'node':'slave1', 'generate_32': 'false'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish release (from release branch name)
Use *branch* parameter instead use last branch release.
```
set BRANCH=release_2023Sep05
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_release', 'node':'slave1', 'generate_32': 'false', 'branch': '%BRANCH%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Close release (from last created release)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '4.close_release', 'node':'slave1'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Close release (from branch name)
Use *branch* parameter instead use last branch release.
```
set BRANCH=release_2023Sep05
curl -H "Content-Type: application/json" -X POST -d "{'name': '4.close_release', 'node':'slave1', 'branch': '%BRANCH%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Promote Beta to Release Candidate
```
set TAG_NAME=Mercury-2023Sep14.2
curl -H "Content-Type: application/json" -X POST -d "{'name': '5.release_beta_to_candidate', 'node':'slave1', 'tag': '%TAG_NAME%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Promote Release Candidate to Gold version
```
set TAG_NAME=Mercury-2023Sep14.2
curl -H "Content-Type: application/json" -X POST -d "{'name': '6.candidate_to_release_GOLD', 'node':'slave1', 'tag': '%TAG_NAME%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Create hotfix (from last created release AND last published version)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '1.create_hotfix', 'node':'slave1'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Create hotfix (from release tag name)
```
set TAG_NAME=Mercury-2023Sep14.2
curl -H "Content-Type: application/json" -X POST -d "{'name': '1.create_hotfix', 'node':'slave1', 'branch': '%TAG_NAME%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish hotfix (from last hotfix branch)
```
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_hotfix', 'node':'slave1'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Publish hotfix (from branch name)
```
set BRANCH=hotfix_2023Sep21.b
curl -H "Content-Type: application/json" -X POST -d "{'name': '3.publish_hotfix', 'node':'slave1', 'branch': '%BRANCH%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```

## Close hotfix (from branch name)
Use *branch* parameter instead use last branch release.
```
set BRANCH=hotfix_2023Sep21.b
curl -H "Content-Type: application/json" -X POST -d "{'name': '4.close_hotfix', 'node':'slave1', 'branch': '%BRANCH%'}" -vs http://cibmcy20n32931v.sanhq.santanderhq.corp:49152/generic-webhook-trigger/invoke?token=mercury
```
