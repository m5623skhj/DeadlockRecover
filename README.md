# DeadlockRecover

## 제작 기간
2025.08.05 ~ 진행중

## 개요

`DeadlockRecover`는 데드락 상황을 해소하고 정상적인 진행을 한다 를 목표로 시작한 프로젝트입니다.

예시:  
워커 스레드 A, B, 직렬화 스레드 C가 있고,  
락 a, b가 A에선 a -> b 순으로, B에선 b -> a 순으로 데드락이 상황이 발생했다고 가정  
이 경우, 데드락을 감지하면 A와 B에서는 현재 진행하고 있는 잡(하나 혹은 N개의 기능)을 롤백 시키고, C에 송신  
C는 이것을 넣어진 순서대로 직접 처리  

---

* ThreadJob
  * 스레드에서 처리할 잡
    * 사용자는 Execute, Commit, Rollback를 구현해야 합니다.
    * 락이 필요할 경우, AcquireLock() 함수를 통해 락을 획득해야 하며, 일정 시간 동안 락을 획득하지 못하면, 데드락으로 판단합니다.
    * 데드락으로 판단되면, DeadlockException 예외를 throw 시키고, Rollback()을 진행한 후, DeaclockRecoverThread에 해당 잡의 처리를 넘깁니다.
* DeadlockRecoverThread
  * 데드락이라고 판단된 ThreadJob을 직렬화시켜 처리합니다.
  * 해당 스레드에 들어온 잡에서의 락은, 타임아웃 되지 않습니다.

---
