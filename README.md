# Numeric Diff Experiment

수치 미분의 전진차분/중심차분 오차가 간격 `h`, 실수 자료형, 함수 종류, `x`의 크기에 따라 어떻게 달라지는지 관찰하기 위한 C 프로그램입니다.

GitHub 저장소 첫 화면에는 README가 보이는 것이 정상입니다. 실제 프로그램은 아래 방법 중 하나로 컴파일해서 실행합니다.

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

GitHub 웹에서 실행하려면 저장소의 **Actions** 탭에서 **Build and run numeric diff experiment** workflow를 실행합니다.
실행이 끝나면 `numeric-diff-results` artifact에서 생성된 CSV 파일을 내려받을 수 있습니다.

## Outputs

실행하면 다음 CSV 파일이 생성됩니다.

- `numeric_diff_data.csv`: 실험 원자료
- `numeric_diff_summary.csv`: 각 조건별 최소 오차와 해당 `h`
