## 목표

C언어로 이름, 학번을 출력하는 프로그램을 각각 만들어서,
crontab 을 이용해 매분마다 프로그램을 실행시켜 이름,학번 출력하도록 만드시오.

crontab -l 의 출력 상태, crontab 의 시간도 함께 보여주기

## 명령어

리눅스에서 단 하나의 crontab 파일이 있고, 이를 수정하므로써 스케쥴링 작업이 가능하다.

```
# crontab 파일을 출력
crontab -l
# crontab 파일 수정하기
crontab -e
# crontab 파일 삭제.
crontab -r
```

### crontab 명령어 추가

- c파이을 컴파일 한 이후 crontab -e 명령어로 다음 내용을 추가한다.

```
* * * * * /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab >> /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab1.log

* * * * * /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab2 >> /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab1.log
```

- crontab -l 으로 출력된 모습

```
ubuntu@ubuntu:~/workspace/DKU-ComputerEngineering/osnw/lab05/dist$ crontab -l
# Edit this file to introduce tasks to be run by cron.
#
# Each task to run has to be defined through a single line
# indicating with different fields when the task will be run
# and what command to run for the task
#
# To define the time you can provide concrete values for
# minute (m), hour (h), day of month (dom), month (mon),
# and day of week (dow) or use '*' in these fields (for 'any').
#
# Notice that tasks will be started based on the cron's system
# daemon's notion of time and timezones.
#
# Output of the crontab jobs (including errors) is sent through
# email to the user the crontab file belongs to (unless redirected).
#
# For example, you can run a backup of all your user accounts
# at 5 a.m every week with:
# 0 5 * * 1 tar -zcf /var/backups/home.tgz /home/
#
# For more information see the manual pages of crontab(5) and cron(8)
#
# m h  dom mon dow   comman
* * * * * /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab >> /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab1.log

* * * * * /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab2 >> /home/ubuntu/workspace/DKU-ComputerEngineering/osnw/lab05/dist/crontab1.log
```

### crontab1.log 확인하기

```
Student ID : 32160462 at Sun Oct 31 07:27:01 2021

Name : 김도영 at Sun Oct 31 07:27:01 2021

Name : 김도영 at Sun Oct 31 07:28:01 2021

Student ID : 32160462 at Sun Oct 31 07:28:01 2021

```
