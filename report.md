# 네트워크 프로그래밍 과제 보고서

**작성자:** 20213107 신성민
**작성일:** 2026-04-09
**개발 환경:** Windows 11, C언어, Winsock2 API

---

## 2. TCP 기반 이미지 전송 서버/클라이언트

### 2.1 프로그램 개요

TCP 소켓을 사용하여 클라이언트가 JPEG 이미지 파일(`pic.jpg`)을 서버로 전송하고, 서버가 이를 수신하여 파일(`Gift.jpg`)로 저장하는 프로그램이다. 포트 번호 **9090**을 사용한다.

---

### 2.2 프로그램 설계 (플로우차트)

#### TCP 서버 (`tcp_server.c`)

```
[시작]
   │
   ▼
[WSAStartup() - Winsock 2.2 초기화]
   │ 실패 → 종료
   ▼
[socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) - TCP 소켓 생성]
   │ 실패 → 종료
   ▼
[sockaddr_in 설정 - INADDR_ANY, PORT 9090]
   │
   ▼
[bind() - 소켓에 IP/포트 바인딩]
   │ 실패 → 종료
   ▼
[listen(backlog=5) - 연결 대기 큐 설정]
   │ 실패 → 종료
   ▼
[accept() - 클라이언트 연결 수락]
   │ 실패 → 종료
   ▼
[recv() - 파일 크기(4바이트) 수신]
   │ ntohl()로 호스트 바이트 순서 변환
   ▼
[malloc(i_size) - 수신 버퍼 동적 할당]
   │
   ▼
┌─[recv() - 이미지 데이터 수신]─┐
│  total_size += n              │
│  total_size < i_size? ────YES─┘
│         NO
│
▼
[fopen("Gift.jpg", "wb") - 파일 열기]
   │
   ▼
[fwrite() - 수신 데이터 파일 저장]
   │
   ▼
[fclose() / free() / closesocket() / WSACleanup()]
   │
   ▼
[종료]
```

#### TCP 클라이언트 (`tcp_client.c`)

```
[시작]
   │
   ▼
[WSAStartup() - Winsock 2.2 초기화]
   │
   ▼
[socket(AF_INET, SOCK_STREAM, 0) - TCP 소켓 생성]
   │
   ▼
[sockaddr_in 설정 - 127.0.0.1, PORT 9090]
   │
   ▼
[connect() - 서버에 연결 요청]
   │ 실패 → 종료
   ▼
[fopen("pic.jpg", "rb") - 전송할 이미지 파일 열기]
   │ 실패 → 종료
   ▼
[fseek(SEEK_END) / ftell() - 파일 크기 측정]
   │
   ▼
[fread() - 파일 전체를 버퍼에 읽기]
   │
   ▼
[htonl(size) - 네트워크 바이트 순서 변환]
   │
   ▼
[send() - 파일 크기(4바이트) 먼저 송신]
   │ 실패 → 종료
   ▼
[send() - 이미지 데이터 전체 송신]
   │ 실패 → 종료
   ▼
[free() / closesocket() / WSACleanup()]
   │
   ▼
[종료]
```

---

### 2.3 핵심 구현 사항

| 항목 | 내용 |
|------|------|
| 소켓 타입 | `SOCK_STREAM` (연결 지향, 신뢰성 보장) |
| 포트 | 9090 |
| 파일 크기 전송 방식 | 4바이트 헤더로 먼저 전송, `htonl()`/`ntohl()`로 바이트 순서 변환 |
| 데이터 수신 방식 | `while` 루프로 누적 수신 (TCP 스트림 특성 대응) |
| 저장 파일명 | `Gift.jpg` |

**바이트 순서 변환 필요성:** 서로 다른 아키텍처 간 통신 시 엔디안 차이로 인한 오류를 방지하기 위해 `htonl()` / `ntohl()` 함수를 사용하여 호스트 ↔ 네트워크 바이트 순서를 변환한다.

**루프 수신의 필요성:** TCP는 스트림 프로토콜이므로 `recv()` 한 번에 전체 데이터가 오지 않을 수 있다. 따라서 `total_size < i_size` 조건의 루프로 전체 데이터를 보장한다.

---

### 2.4 기능 검증 방법 및 환경

#### 검증 환경

| 항목 | 내용 |
|------|------|
| OS | Windows 11 Pro |
| 언어 | C (Winsock2) |
| 빌드 도구 | CMake + MSVC |
| 실행 환경 | 로컬 루프백 (`127.0.0.1`) |
| 테스트 이미지 | `pic.jpg` (임의의 JPEG 파일) |

#### 검증 방법

1. **서버 먼저 실행** — `tcp_server.exe`를 실행하여 포트 9090에서 연결 대기
2. **클라이언트 실행** — `tcp_client.exe`를 실행하여 `pic.jpg` 전송
3. **수신 파일 확인** — 서버 실행 디렉토리에 `Gift.jpg`가 생성되었는지 확인
4. **파일 무결성 검증** — 원본 `pic.jpg`와 `Gift.jpg`의 파일 크기 및 이미지 내용 비교

---

### 2.5 기능 검증 결과

| 검증 항목 | 결과 |
|-----------|------|
| 서버 소켓 생성 및 바인딩 | 정상 |
| 클라이언트 연결 수립 (3-way handshake) | 정상 |
| 파일 크기 헤더 송수신 | 정상 |
| 이미지 데이터 전체 수신 (루프 누적) | 정상 |
| `Gift.jpg` 파일 생성 및 저장 | 정상 |
| 원본과 수신 파일 동일성 | **일치** |

서버 측 콘솔에는 누적 수신 바이트 수가 출력되며, 전체 수신 완료 후 `"Image Get"` 메시지가 출력된다.

```
// 서버 실행 결과 예시
4096
8192
...
[파일 전체 크기]
Image Get
```

---

## 3. UDP 기반 이미지 전송 서버/클라이언트

### 3.1 프로그램 개요

UDP 소켓을 사용하여 클라이언트가 `pic.jpg`를 **4096바이트 청크(chunk) 단위**로 분할 전송하고, 서버가 이를 수신하여 `Gift_udp.jpg`로 저장하는 프로그램이다. 포트 번호 **9091**을 사용한다.

---

### 3.2 프로그램 설계 (플로우차트)

#### UDP 서버 (`udp_server.c`)

```
[시작]
   │
   ▼
[WSAStartup() - Winsock 초기화]
   │
   ▼
[socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) - UDP 소켓 생성]
   │
   ▼
[bind() - PORT 9091 바인딩]
   │ ※ listen/accept 없음 (비연결형)
   ▼
[recvfrom() - 파일 크기(4바이트) 수신]
   │ 클라이언트 주소(addr_c) 자동 획득
   │ ntohl()로 변환
   ▼
[malloc(i_size) - 수신 버퍼 동적 할당]
   │
   ▼
┌─[recvfrom(CHUNK=4096) - 청크 단위 수신]──┐
│  total_size += n, chunk_idx++            │
│  청크 번호/수신량 출력                    │
│  total_size < i_size? ───────────YES─────┘
│         NO
│
▼
[fopen("Gift_udp.jpg", "wb")]
   │
   ▼
[fwrite() - 파일 저장]
   │
   ▼
[free() / closesocket() / WSACleanup()]
   │
   ▼
[종료]
```

#### UDP 클라이언트 (`udp_client.c`)

```
[시작]
   │
   ▼
[WSAStartup() - Winsock 초기화]
   │
   ▼
[socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) - UDP 소켓 생성]
   │ ※ connect() 없음 (비연결형)
   ▼
[sockaddr_in 설정 - 127.0.0.1, PORT 9091]
   │
   ▼
[fopen("pic.jpg", "rb") - 파일 열기]
   │
   ▼
[fseek/ftell - 파일 크기 측정 후 fread() 전체 읽기]
   │
   ▼
[sendto() - 파일 크기(4바이트) 먼저 송신]
   │ htonl()로 변환
   ▼
┌─[sendto(CHUNK=4096) - 청크 단위 분할 송신]──┐
│  offset += send_size, chunk_idx++           │
│  offset < size? ──────────────────YES───────┘
│         NO
│
▼
[free() / closesocket() / WSACleanup()]
   │
   ▼
[종료]
```

---

### 3.3 TCP 기반 프로그램과의 차이점

| 비교 항목 | TCP 프로그램 | UDP 프로그램 |
|-----------|-------------|-------------|
| **소켓 타입** | `SOCK_STREAM` | `SOCK_DGRAM` |
| **연결 설정** | `connect()` / `accept()` 필요 (3-way handshake) | 불필요 (비연결형) |
| **데이터 송신 함수** | `send()` | `sendto()` (매 호출마다 대상 주소 지정) |
| **데이터 수신 함수** | `recv()` | `recvfrom()` (송신자 주소 함께 수신) |
| **listen() 사용** | 필요 | 불필요 |
| **전송 단위** | 스트림 (경계 없음) | 데이터그램 (4096바이트 청크) |
| **신뢰성** | OS 레벨에서 재전송/순서 보장 | 보장 없음 (손실/순서 바뀜 가능) |
| **포트** | 9090 | 9091 |
| **저장 파일명** | `Gift.jpg` | `Gift_udp.jpg` |

#### 주요 구조적 차이

**1. 연결 과정 생략**

TCP는 `listen()` → `accept()`로 클라이언트와 1:1 연결을 수립한 뒤 통신하지만, UDP는 `bind()` 후 곧바로 `recvfrom()`으로 데이터를 수신한다. 서버가 클라이언트 주소를 `recvfrom()`의 반환값으로 자동으로 알게 된다.

**2. 청크 분할 전송**

TCP는 스트림 방식이라 한 번의 `send()`로 대용량 데이터를 전송하더라도 OS가 내부적으로 분할·재조합한다. 반면 UDP는 데이터그램 크기 제한(MTU)이 있으므로 클라이언트가 직접 **4096바이트씩 분할**하여 반복 전송하고, 서버도 청크 단위로 반복 수신한다.

**3. 신뢰성 처리**

TCP는 프로토콜 레벨에서 손실 패킷 재전송 및 순서 보장을 제공하지만, 현재 UDP 구현에는 손실/재전송 처리 로직이 없다. 따라서 로컬 루프백(`127.0.0.1`) 환경에서는 정상 동작하나, 실제 네트워크에서는 패킷 손실 시 수신 파일이 손상될 수 있다.

---

*본 보고서는 `F:/Campus/3_1/network` 프로젝트의 소스코드(`tcp_server.c`, `tcp_client.c`, `udp_server.c`, `udp_client.c`) 분석을 기반으로 작성되었다.*
