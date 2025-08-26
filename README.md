# DeadlockRecover

**2025.08.05 ~ 2025.08.27**

`DeadlockRecover`는 '데드락 상황을 해소하고 정상적인 진행을 한다'를 목표로 시작한 프로젝트입니다.

## 개요
- 락을 획득할 때, 타임아웃이 되면 데드락으로 판단
- 데드락 감지 시 자동으로 작업 롤백 및 재시도
- 복구 전용 스레드에서 순차 처리로 데드락 해결

## 사용 방법
```cpp
class MyJob : public ThreadJob
{
    void Execute() override
    {
        // 작업 수행
    }
    
    void Commit() override
    {
        // 커밋
    }
    
    void Rollback() override
    {
        // 롤백 처리
    }
};

// 사용 이전에 반드시 데드락 복구 스레드를 구동 시켜줘야 합니다.
DeadlockRecoverThread::GetInstance().Start();

// ... 

auto job = ThreadJob::CreateJob<MyJob>(mutexA, mutexB);
job->Do();
```

## 핵심 기능
- **ThreadJob**: 작업 단위 추상 클래스
  - Execute(), Commit(), Rollback() 구현 필요
  - 락 획득 시, AcquireLock()을 통한 타임아웃 기반 락 획득
  - 타임아웃 시 DeadlockException 발생 및 자동 롤백
- **DeadlockRecoverThread**: 데드락 복구 전용 스레드
  - 실패한 작업을 큐에 저장하여 순차 처리
  - 타임아웃이 없이 락을 획득하고 실행을 보장

## 동작 원리
1. 워커 스레드에서 락 획득 시 설정된 타임아웃 내에 획득 실패 시 데드락으로 판단
2. 현재 작업을 롤백하고 DeadlockRecoverThread에 재시도 요청
3. 복구 스레드에서 타임아웃 없이 순차 실행하여 데드락 해결

## 요구사항
- C++20 이상
- `std::timed_mutex`, `std::jthread`, `std::counting_semaphore` 지원
