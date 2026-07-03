# EternalReturn — Unreal Engine 모작

<img width="977" height="595" alt="image" src="https://github.com/user-attachments/assets/0a2317ff-450a-48f8-b5a6-e82395bbb435" />


> 이터널 리턴(Eternal Return)을 Unreal Engine 5.7로 재현한 포트폴리오 프로젝트입니다.

---

## 개요

| 항목 | 내용 |
|---|---|
| 장르 | 탑다운 서바이벌 액션 (Battle Royale + MOBA 하이브리드) |
| 엔진 | Unreal Engine 5.7 |
| 언어 | C++ / Blueprint 혼합 |
| 네트워크 | Listen Server + 자체 TCP 백엔드 |
| 개발 기간 | 2026.05 ~ 2026.07 |
| 개발 인원 | 1인 |

평소 즐겨 하던 게임 이터널 리턴을 플레이하면서 "이 게임은 어떤 구조로 만들어졌을까"라는 궁금증에서 시작한 프로젝트입니다. 전투/스킬, 인벤토리, 시야 시스템, 몬스터 AI, 로비/매칭 등 상용 게임 수준의 시스템을 밑바닥부터 직접 설계·구현하며 멀티플레이어 게임 개발 파이프라인 전반을 경험하는 것을 목표로 했습니다. 특히 이터널 리턴 특유의 시야 기반 정보전과 실시간 전투/스킬 시스템을 얼마나 근접하게 재현할 수 있을지에 집중했습니다.

---

## 기술 스택

- **C++** — 핵심 시스템 (스탯/컴포넌트 계층, 스킬 로직, 네트워크 리플리케이션)
- **Blueprint** — 캐릭터별 스킬 연출, UI, 레벨 로직
- **TCP 백엔드** — 별도 C++ 콘솔 앱, Winsock2 + nlohmann/json + MySQL 8.4
- **Replication System** — Listen Server 기반 상태 동기화 (OnRep 중심)
- **Behavior Tree / NavMesh** — 몬스터 AI, 경로 탐색 및 클릭 이동

---

## 아키텍처

로그인/로비/룸 매칭은 UE 네트워크 세션이 아닌 **별도 TCP 백엔드**를 통해 처리하고, 실제 게임 플레이는 **Listen Server**로 동작합니다.

```
[클라이언트] --TCP--> [백엔드 서버 (로그인 / 룸코드 / MySQL)]
                              |
                     방 코드로 host IP·Port 교환
                              |
[클라이언트] --UE Listen Server 접속(OpenLevel)--> [호스트 = 서버 겸 클라이언트]
```

- 메인 로비는 TCP 클라이언트 화면으로만 동작 (UE 네트워크 세션 아님)
- 호스트가 캐릭터 선택 레벨에 진입하는 시점에 Listen Server 시작
- 클라이언트는 6자리 방 코드 입력 → 백엔드가 호스트 IP/Port 반환 → `OpenLevel` 접속
- 범위: LAN 플레이 (NAT 트래버설은 추후 과제)

---

## 구현 내용

### 캐릭터 / 전투 시스템
- `UBaseStatComponent` 계층으로 캐릭터/몬스터 스탯 공통화, HP/공격력/방어력 리플리케이션
- 방어력 공식 등 원작 전투 수치 재현

### 스킬 시스템
- `USkillComponent`가 쿨타임·스킬 레벨·사용 가능 여부를 전담, 실제 연출은 캐릭터별 Blueprint에서 오버라이드
- `UDA_SkillBase` DataAsset의 `SkillType` Enum(Projectile/Dash/AreaDamage/Buff/CC/Toggle/Passive)으로 스킬 종류 분기
- Q/W/E/R(5레벨) · T(패시브 3레벨) · D(무기 스킬) · F(전술 스킬) 원작 키 구조 재현

### 인벤토리 / 장비 시스템
- 10슬롯 인벤토리 + 5슬롯 장비, `OnRep` 기반 리플리케이션으로 UI 동기화
- 무기 타입 제한, 장착 시 스탯 연동

### 구조물 / 루팅 시스템
- `ItemList(Replicated)`는 구조물이 소유, 추가/제거는 GameMode가 경유 처리
- 루팅: 슬롯 클릭 → `ServerTakeItem` → `TakeItemFromStructure` → `InventoryComponent.AddItem`
  
### 시야 / 포그오브워
- `SceneCaptureComponent2D` + 렌더타겟으로 시야 밖 월드 암전
- `VisionDetectionComponent`의 스피어 오버랩으로 시야 범위 밖 액터 은닉

### 몬스터 AI
- Behavior Tree (BB/BT/BTS_Monster) 기반 추격, HP 기반 귀환 로직

### 캐릭터 선택 → 인게임 전환
- Seamless Travel 기반 캐릭터 선택 → 스폰 파이프라인

### 제작 / 루팅 시스템
- `UCraftingComponent` 기반 제작, 구조물 루팅 UI

### 로그인 / 로비
- TCP 백엔드 연동 로그인/회원가입 UI, `BackendConnectionSubsystem`으로 레벨 전환 간 TCP 연결 유지

---

## 하이라이트

### 스킬 컴포넌트

**문제** — 이터널 리턴 원작의 스킬 체계를 캐릭터마다 다른 로직을 가지면서도 하나의 공용 컴포넌트로 관리해야 했습니다.

**설계**
- `USkillComponent`가 쿨타임/레벨/사용 가능 여부를 전담하고, 실제 연출은 캐릭터별 Blueprint에서 오버라이드하는 구조로 분리
- `TargetLocation`(마우스 클릭 위치)을 `ExecuteSkill → UseWeaponSkill/UseTacticalSkill → SkillType() → 타입별 함수`까지 하나의 파라미터로 관통시켜 모든 스킬 타입의 진입점을 통일

**트러블슈팅**
| 문제 | 원인 | 해결 |
|---|---|---|
| 3연속 공격 몽타주 미재생 | `bUseToggle`이 Replicated 누락 / AnimGraph Slot 연결 누락 / Play Rate가 공격속도 대비 부족 | 3가지 원인 각각 수정 |
| F/D 키 무반응 | `ExecuteSkill()`이 Q/W/E/R만 처리 | F→`UseTacticalSkill()`, D→`UseWeaponSkill()`로 라우팅 분리 |
| Blink 사거리 이상 | DataTable Range=3.0 → 실제 약 3cm (단위 착오) | Range 값 수정 |


<img width="392" height="220" alt="Skill" src="https://github.com/user-attachments/assets/128a905b-20d3-437d-9bd4-a3b7872063a3" />


---

### 인벤토리 컴포넌트

**문제** — Dedicated Server는 서버 프로세스에 HUD/Widget이 존재하지 않는데, 초기 구조는 UI 갱신 함수가 `BP_Character`에 있어 서버에서 접근할 대상이 없었습니다.

**설계** — `Multicast RPC` 방식에서 `ReplicatedUsing`(`OnRep_InventorySlots`, `OnRep_EquipSlots`) 방식으로 전환. Multicast는 서버가 모든 클라이언트에 무조건 브로드캐스트하는 구조라 결합도가 높은 반면, OnRep은 값이 바뀐 클라이언트에서만 자동 호출되어 서버 로직과 분리하기 좋았습니다.

**트러블슈팅**
- UI 갱신 함수(`InventorySlotsUpdate`, `EquipSlotsUpdate`)를 `BP_Character` → `WBP_HUD`로 이동
- 델리게이트 바인딩을 `WBP_HUD`의 `Event Construct`로 옮겨 위젯이 존재하는 클라이언트에서만 바인딩되도록 수정
- `BP_PlayerController`의 `OnPossess`를 HUD 참조 설정만 담당하도록 단순화

**구조물(루팅)과의 연동**
- 인벤토리로 아이템이 들어오는 경로 중 하나가 구조물 루팅
- 구조물 ItemList는 구조물이 소유(Replicated), 추가/제거는 GameMode가 처리
- 흐름: 슬롯 클릭 → ServerTakeItem → TakeItemFromStructure(GameMode) → ItemList 제거 + InventoryComponent.AddItem


<img width="446" height="250" alt="Inven" src="https://github.com/user-attachments/assets/5a2fd060-3abb-48b2-a441-536fe273e56c" />


---

### 구조물 시스템

**문제** — 루팅 가능한 구조물(상자 등)의 아이템 목록을 어디서 관리하고, 여러 클라이언트가 동시에 열었을 때 UI를 어떻게 동기화할지가 핵심이었습니다.

**설계**
- `ItemList(Replicated)`는 구조물이 소유, `OnRep`으로 UI 자동 갱신
- 추가/제거는 `GameMode`가 처리 (구조물은 Owner 불명확 → RPC 라우팅 문제 방지)
- 흐름: 슬롯 클릭 → `ServerTakeItem` → `TakeItemFromStructure(GameMode)` → `ItemList` 제거 + `InventoryComponent.AddItem`
- `WBP_LootBox`는 `StructureRef` 기준으로 바인딩/언바인딩


**트러블슈팅**

- 아이템 클릭 시 다른 클라이언트에는 즉시 사라지는데, 클릭한 본인 화면에는 한 번 더 클릭해야 사라지는 버그

- 원인 1: `S2C_OpenLootBox`가 `SetOwner(CastPC)`를 호출하지 않아 `Dedicated Server`가 `Client RPC`를 본인 클라이언트로 라우팅하지 못함 → `SetOwner(CastPC)` 추가로 해결
- 원인 2: `InitSlots가 ItemList`를 파라미터로 받아 처리해서 최초 변경 시 `OnRep`이 발동하지 않음 → `self.ItemList`를 직접 참조하도록 수정


<img width="334" height="188" alt="rnwhanf" src="https://github.com/user-attachments/assets/aa2830f9-4823-4b24-b244-9139df602235" />


---

### 시야 / 포그오브워 시스템

**설계** — 두 개 레이어로 구성
- **레이어 1 (월드 암전)**: `SceneCaptureComponent2D` + `RT_VisionMask` 렌더타겟으로 시야 밖 월드를 어둡게 처리
- **레이어 2 (액터 은닉)**: `VisionDetectionComponent`가 스피어 오버랩으로 시야 범위 내 액터만 노출

**트러블슈팅** — `IsLocallyControlled` 타이밍 레이스 컨디션 (`PossessedBy` / `OnRep_Controller` C++ 오버라이드 순서 문제). Listen Server 전환 이후 재검토 예정.


<img width="476" height="268" alt="tldi" src="https://github.com/user-attachments/assets/eacb99c0-2a99-4c93-8c0a-d7655989c091" />


---

### 캐릭터 선택 → 인게임 전환

**설계**
- `GameMode::BeginPlay`, `HandleStartingNewPlayer`는 서버 사이드에서만 실행. Seamless Travel 중인 플레이어는 `K2_PostLogin`이 호출되지 않아 `HandleStartingNewPlayer`를 공통 훅으로 사용
- `SpawnIndex`를 클래스 변수로 관리해 호출 간 초기화 방지
- InGame GameMode의 `Default Pawn Class`를 `None`으로 설정해 커스텀 스폰 로직 이전 자동 스폰 방지
- `DefaultEngine.ini`의 `[ConsoleVariables]`에 `net.AllowPIESeamlessTravel=1` 추가 필요

**트러블슈팅** — 트랜지션 맵 로딩 위젯이 PIE에서 보이지 않음 (에디터 애셋 캐싱으로 전환이 1ms 미만 처리됨). PIE 한정 이슈로 확인, 패키징 빌드에서 재검증 예정.


<img width="412" height="232" alt="Pick" src="https://github.com/user-attachments/assets/0744f749-953d-4078-b4e2-0551448c9375" />


---

## 트러블슈팅 / 배운 점

- OnRep 콜백은 서버 자신에게는 호출되지 않으므로, 서버에서 리플리케이트 배열을 수정할 때는 수동으로 `Broadcast()`를 함께 호출해야 함
- `AddDynamic` 델리게이트 바인딩은 `Super::BeginPlay()` 이전에 배치해야 `ReceiveBeginPlay()` 중 발생하는 브로드캐스트를 놓치지 않음
- Listen Server 환경에서는 호스트가 서버와 로컬 플레이어를 겸하기 때문에, Dedicated Server(로컬 플레이어 없음)에서는 드러나지 않던 버그가 노출됨
- Character에 부착된 컴포넌트의 Client RPC는 정상 라우팅되지 않아, PlayerController를 경유해서 처리
- `bIsCrafting` 등 상태 플래그는 클라이언트 측 체크가 정상 동작하려면 `Replicated` 지정 필요
- MySQL C API: `mysql_real_escape_string`은 `std::vector<char>` 버퍼 필요, 스레드 핸들러엔 `mysql_thread_init()`/`mysql_thread_end()` 필요, `mysql_store_result()` null 체크 필요
- `nlohmann::json`은 잘못된 UTF-8에 예외를 던지므로 `Response.dump()`는 try/catch로 감싸야 함

---

## 진행 중 / 앞으로 할 것

- 승리 조건 → 결과 화면 → 로비 복귀 플로우
- 대기방(프라이빗 룸) PlayerSlot 입장/퇴장 리플리케이션
- 미니맵, Tab 스코어보드
- 남은 전술 스킬 스텁 구현
- 비밀번호 해싱, 중복 로그인 방지

---

## 실행 방법

1. 프로젝트 클론
2. `EternalReturn.uproject` 우클릭 → `Generate Visual Studio project files`
3. Visual Studio에서 빌드 (`Ctrl + Shift + B`)
4. 언리얼 에디터 실행

---

## 스크린샷 / 영상

추가 예정
