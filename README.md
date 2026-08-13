# Numeric Diff Experiment

수치 미분의 전진차분/중심차분 오차가 간격 `h`, 실수 자료형, 함수 종류, `x`의 크기에 따라 어떻게 달라지는지 관찰하기 위한 C 프로그램입니다.

## Build

```sh
gcc -O2 -std=c11 -Wall -o numeric_diff_experiment numeric_diff_experiment.c -lm
```

## Run

Linux/macOS:

```sh
./numeric_diff_experiment
```

Windows:

```bat
numeric_diff_experiment.exe
```

## Outputs

실행하면 다음 CSV 파일이 생성됩니다.

- `numeric_diff_data.csv`: 실험 원자료
- `numeric_diff_summary.csv`: 각 조건별 최소 오차와 해당 `h`
