# Eternal Return — Unreal Engine 모작

> 이터널 리턴(Eternal Return)을 언리얼 엔진으로 재현한 포트폴리오 프로젝트입니다.

---

## 개요

| 항목 | 내용 |
|---|---|
| 장르 | 탑다운 생존 액션 (Battle Royale + MOBA) |
| 엔진 | Unreal Engine 5.7 |
| 언어 | C++ / Blueprint 혼합 |
| 네트워크 | Listen Server |
| 개발 기간 | 2025.05 ~ |
| 개발 인원 | 1인 |

---

## 기술 스택

- **C++** — 핵심 시스템 (클래스 계층, 네트워크, 전투)
- **Blueprint** — 스킬 로직, 애니메이션, UI
- **Unreal Engine NavMesh** — 경로 탐색 및 클릭 이동
- **Replication System** — 리슨서버 기반 상태 동기화

---

## 구현 내용

### 캐릭터 시스템
- `ACombatEntityBase` 를 공통 베이스로 캐릭터 / 몬스터 계층 구조 설계
- HP, 공격력, 방어력 등 스탯 리플리케이션
- `UENUM + TArray` 기반 상태이상 시스템

### 이동 시스템
- NavMesh 경로 계산 + `AddMovementInput` 방식의 클릭 이동 구현
- `SimpleMoveToLocation` 의 AI 이동 방식 한계 분석 및 교체

### 네트워크
- 데미지 처리 서버 권한 분리 (`HasAuthority`)
- `ReplicatedUsing` 을 통한 HP / 상태이상 동기화

---

## 실행 방법
1.프로젝트 클론 \n
2.EternalReturn.uproject 우클릭 → Generate Visual Studio project files \n
3.Visual Studio에서 빌드 (Ctrl + Shift + B) \n
4.언리얼 에디터 실행 \n

---

## 스크린샷

추가 예정
