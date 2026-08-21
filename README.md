# Numeric Diff Experiment

전진차분과 중심차분으로 수치 미분을 수행하면서 `h`, 실수 자료형, 함수 종류, `x`의 크기에 따라 오차가 어떻게 변하는지 관찰하는 실험입니다.

## 웹에서 바로 실행

아래 GitHub Pages 주소에서 브라우저 UI로 바로 실험할 수 있습니다.

<https://kimyesung0215-blip.github.io/numeric-diff-experiment-c/>

웹 UI에서는 **10회 계산**으로 값을 10개씩 누적해 그래프 변화를 관찰하거나, **총 계산**으로 전체 실험을 한 번에 완료할 수 있습니다. **초기화**를 누르면 계산 상태와 그래프가 처음으로 돌아갑니다.
결과 분석 탭의 **그래프 해석** 스위치를 켜면 그래프별 해석 문장을 함께 볼 수 있습니다.

계산 후에는 다음을 확인할 수 있습니다.

- **결과 요약**: 조건별 최소 오차와 그때의 `h`
- **결과 분석**: `10^-1`부터 `10^-18`까지의 `h`-오차 곡선, 자료형별 정밀도 표, 오차 감소비 표, 함수와 `x` 크기별 비교 그래프
- **연구 질문별 해석**: `h`를 줄일수록 항상 정확해지는지, 오차가 다시 증가하는 이유, 자료형/차분법/함수별 차이
- **CSV 다운로드**: 원자료와 요약 자료

브라우저의 JavaScript는 기본적으로 64비트 부동소수점 계산을 사용하므로, 웹 UI의 `long double`은 C 실행 파일의 실제 `long double` 정밀도와 완전히 같지는 않습니다. 정밀한 자료형 비교가 필요하면 아래 C 프로그램 실행 결과를 함께 확인하세요.

## Windows에서 실행

PowerShell:

```powershell
.\run.ps1
```

또는 명령 프롬프트:

```bat
run.bat
```

## Linux/macOS에서 실행

```sh
chmod +x run.sh
./run.sh
```

또는:

```sh
make run
```

## 직접 컴파일

```sh
gcc -O2 -std=c11 -Wall -o numeric_diff_experiment numeric_diff_experiment.c -lm
```

실행:

```sh
./numeric_diff_experiment
```

Windows에서는:

```bat
numeric_diff_experiment.exe
```

## GitHub에서 실행

GitHub 저장소의 **Actions** 탭에서 **Build and run numeric diff experiment** workflow를 실행할 수 있습니다.

실행이 끝나면 `numeric-diff-results` artifact에서 생성된 CSV 파일을 내려받을 수 있습니다.

## Outputs

실행하면 다음 CSV 파일이 생성됩니다.

- `numeric_diff_data.csv`: 실험 원자료
- `numeric_diff_summary.csv`: 각 조건별 최소 오차와 해당 `h`
