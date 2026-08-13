/*
 * numeric_diff_experiment.c
 *
 * [탐구 목적]
 * 수치 미분(전진차분/중심차분)의 오차가 h, 실수 자료형(float/double/long double),
 * 함수의 종류, x의 크기에 따라 어떻게 변하는지를 관찰하기 위한 "원자료 생성" 프로그램.
 * 이 프로그램은 "어떤 자료형이 가장 좋다"와 같은 결론을 스스로 내리지 않는다.
 * 결론은 CSV 데이터를 분석하는 사람(학생)의 몫이다.
 *
 * [생성 파일]
 *  - numeric_diff_data.csv    : 실험 원자료 (h, 오차 등)
 *  - numeric_diff_summary.csv : 각 (실험,자료형,함수,방법)별 최소 오차와 그때의 h
 *
 * 컴파일 (GCC, Windows/Linux 공통):
 *   gcc -O2 -std=c11 -Wall -o numeric_diff_experiment numeric_diff_experiment.c -lm
 *
 * 실행:
 *   ./numeric_diff_experiment        (Linux/Mac)
 *   numeric_diff_experiment.exe      (Windows)
 */

#include <stdio.h>
#include <math.h>
#include <float.h>

/* ------------------------------------------------------------------
 * 출력 정밀도 설정
 * float  : 유효숫자 약 6~7자리 -> 여유있게 9자리까지 출력
 * double : 유효숫자 약 15~17자리 -> 왕복 변환이 보장되는 17자리 출력
 * long double : 플랫폼에 따라 다름(더블과 같을 수도, 80비트 확장정밀도일 수도 있음)
 *               -> 넉넉하게 21자리까지 출력(불필요한 자리는 그냥 0/잡음으로 보이므로 무해함)
 * ------------------------------------------------------------------ */
#define FLOAT_PREC   9
#define DOUBLE_PREC  17
#define LDOUBLE_PREC 21

/* h를 10배씩 줄여나갈 때 무한 반복을 막기 위한 최대 반복 횟수(안전장치) */
#define MAX_STEPS 400

/* 실험에서 사용할 함수 종류를 나타내는 열거형 (복잡한 구조 없이 switch로만 사용) */
typedef enum {
    FUNC_X2 = 0,   /* f(x) = x^2      , f'(x) = 2x     */
    FUNC_SIN = 1,  /* f(x) = sin(x)   , f'(x) = cos(x) */
    FUNC_EXP = 2,  /* f(x) = exp(x)   , f'(x) = exp(x) */
    FUNC_LOG = 3   /* f(x) = log(x)   , f'(x) = 1/x    */
} FunctionType;

/* CSV에 기록할 함수 이름 문자열을 반환 */
static const char *function_name(FunctionType ft)
{
    switch (ft) {
        case FUNC_X2:  return "x^2";
        case FUNC_SIN: return "sin(x)";
        case FUNC_EXP: return "exp(x)";
        case FUNC_LOG: return "log(x)";
    }
    return "unknown";
}

/* ==================================================================
 * float 전용 함수 (sinf/cosf/expf/logf/fabsf만 사용, double 섞지 않음)
 * ================================================================== */
static float eval_f(FunctionType ft, float x)
{
    switch (ft) {
        case FUNC_X2:  return x * x;
        case FUNC_SIN: return sinf(x);
        case FUNC_EXP: return expf(x);
        case FUNC_LOG: return logf(x);
    }
    return 0.0f;
}
static float true_deriv_f(FunctionType ft, float x)
{
    switch (ft) {
        case FUNC_X2:  return 2.0f * x;
        case FUNC_SIN: return cosf(x);
        case FUNC_EXP: return expf(x);
        case FUNC_LOG: return 1.0f / x;
    }
    return 0.0f;
}

/* ==================================================================
 * double 전용 함수 (sin/cos/exp/log/fabs 사용)
 * ================================================================== */
static double eval_d(FunctionType ft, double x)
{
    switch (ft) {
        case FUNC_X2:  return x * x;
        case FUNC_SIN: return sin(x);
        case FUNC_EXP: return exp(x);
        case FUNC_LOG: return log(x);
    }
    return 0.0;
}
static double true_deriv_d(FunctionType ft, double x)
{
    switch (ft) {
        case FUNC_X2:  return 2.0 * x;
        case FUNC_SIN: return cos(x);
        case FUNC_EXP: return exp(x);
        case FUNC_LOG: return 1.0 / x;
    }
    return 0.0;
}

/* ==================================================================
 * long double 전용 함수 (sinl/cosl/expl/logl/fabsl만 사용)
 * 리터럴도 반드시 L 접미사를 붙여 double로 강등되지 않게 한다.
 * ================================================================== */
static long double eval_l(FunctionType ft, long double x)
{
    switch (ft) {
        case FUNC_X2:  return x * x;
        case FUNC_SIN: return sinl(x);
        case FUNC_EXP: return expl(x);
        case FUNC_LOG: return logl(x);
    }
    return 0.0L;
}
static long double true_deriv_l(FunctionType ft, long double x)
{
    switch (ft) {
        case FUNC_X2:  return 2.0L * x;
        case FUNC_SIN: return cosl(x);
        case FUNC_EXP: return expl(x);
        case FUNC_LOG: return 1.0L / x;
    }
    return 0.0L;
}

/* ------------------------------------------------------------------
 * <float.h> 환경 정보 출력
 * long double이 이 컴파일 환경에서 실제로 double보다 정밀한지는
 * 프로그램이 판단하지 않고, 값만 그대로 보여준다(판단은 학생 몫).
 * ------------------------------------------------------------------ */
static void print_environment_info(void)
{
    printf("========== 컴파일 환경의 실수 자료형 정보 ==========\n");
    printf("sizeof(float)       = %zu byte\n", sizeof(float));
    printf("sizeof(double)      = %zu byte\n", sizeof(double));
    printf("sizeof(long double) = %zu byte\n", sizeof(long double));
    printf("\n");
    printf("FLT_DIG  (float  10진 유효자리 보장 개수)  = %d\n", FLT_DIG);
    printf("DBL_DIG  (double 10진 유효자리 보장 개수)  = %d\n", DBL_DIG);
    printf("LDBL_DIG (long double 10진 유효자리 보장 개수) = %d\n", LDBL_DIG);
    printf("\n");
    printf("FLT_MANT_DIG  (float  가수부 비트 수)  = %d\n", FLT_MANT_DIG);
    printf("DBL_MANT_DIG  (double 가수부 비트 수)  = %d\n", DBL_MANT_DIG);
    printf("LDBL_MANT_DIG (long double 가수부 비트 수) = %d\n", LDBL_MANT_DIG);
    printf("\n");
    printf("FLT_EPSILON  = %.*e\n", FLOAT_PREC, (double)FLT_EPSILON);
    printf("DBL_EPSILON  = %.*e\n", DOUBLE_PREC, DBL_EPSILON);
    printf("LDBL_EPSILON = %.*Le\n", LDOUBLE_PREC, LDBL_EPSILON);
    printf("=====================================================\n\n");
}

/* ==================================================================
 * run_float / run_double / run_long_double
 *
 * 하나의 (실험이름, 자료형, 함수, 시작 x)에 대해 h를 0.1부터 10배씩 줄여가며
 * 전진차분(do_forward) / 중심차분(do_central)을 계산하고 CSV에 기록한다.
 *
 * 안전장치:
 *   - 다음 h(=h/10)가 0이 되거나, 더 이상 h와 달라지지 않으면(부동소수점 표현의 한계)
 *     반복을 종료한다.
 *   - 위 조건과 별개로 MAX_STEPS를 넘지 않도록 강제 종료한다.
 *
 * x+h==x, x-h==x 비교는 컴파일러가 "당연히 다르다"고 상수 취급해 최적화로
 * 지워버리지 않도록 volatile 변수를 거쳐서 계산한다.
 * (단, -ffast-math 옵션은 IEEE754 규칙을 깨뜨리므로 사용하지 않아야 한다.)
 * ================================================================== */

static void run_float(FILE *data, FILE *summary,
                       const char *experiment, FunctionType ft, float x,
                       int do_forward, int do_central)
{
    const char *fname = function_name(ft);
    float h = 0.1f;

    int   have_fwd = 0, have_ctr = 0;
    float min_err_fwd = 0.0f, min_h_fwd = 0.0f;
    float min_err_ctr = 0.0f, min_h_ctr = 0.0f;

    for (int step = 0; step < MAX_STEPS; step++) {
        volatile float vx  = x;
        volatile float vh  = h;
        volatile float vxp = vx + vh;
        int eq_plus = (vxp == vx);

        float true_d = true_deriv_f(ft, x);

        if (do_forward) {
            float num_d = (eval_f(ft, x + h) - eval_f(ft, x)) / h;
            float err = fabsf(num_d - true_d);
            fprintf(data, "%s,float,%s,forward,%.*e,%.*e,%.*e,%.*e,%.*e,%d,NA\n",
                    experiment, fname,
                    FLOAT_PREC, x, FLOAT_PREC, h,
                    FLOAT_PREC, num_d, FLOAT_PREC, true_d, FLOAT_PREC, err,
                    eq_plus);
            if (!have_fwd || err < min_err_fwd) { min_err_fwd = err; min_h_fwd = h; have_fwd = 1; }
        }
        if (do_central) {
            volatile float vxm = vx - vh;
            int eq_minus = (vxm == vx);
            float num_d = (eval_f(ft, x + h) - eval_f(ft, x - h)) / (2.0f * h);
            float err = fabsf(num_d - true_d);
            fprintf(data, "%s,float,%s,central,%.*e,%.*e,%.*e,%.*e,%.*e,%d,%d\n",
                    experiment, fname,
                    FLOAT_PREC, x, FLOAT_PREC, h,
                    FLOAT_PREC, num_d, FLOAT_PREC, true_d, FLOAT_PREC, err,
                    eq_plus, eq_minus);
            if (!have_ctr || err < min_err_ctr) { min_err_ctr = err; min_h_ctr = h; have_ctr = 1; }
        }

        float next_h = h / 10.0f;
        if (next_h == 0.0f || next_h == h) break;  /* 더 이상 유의미하게 줄어들지 않음 */
        h = next_h;
    }

    if (do_forward)
        fprintf(summary, "%s,float,%s,forward,%.*e,%.*e\n",
                experiment, fname, FLOAT_PREC, min_h_fwd, FLOAT_PREC, min_err_fwd);
    if (do_central)
        fprintf(summary, "%s,float,%s,central,%.*e,%.*e\n",
                experiment, fname, FLOAT_PREC, min_h_ctr, FLOAT_PREC, min_err_ctr);
}

static void run_double(FILE *data, FILE *summary,
                        const char *experiment, FunctionType ft, double x,
                        int do_forward, int do_central)
{
    const char *fname = function_name(ft);
    double h = 0.1;

    int    have_fwd = 0, have_ctr = 0;
    double min_err_fwd = 0.0, min_h_fwd = 0.0;
    double min_err_ctr = 0.0, min_h_ctr = 0.0;

    for (int step = 0; step < MAX_STEPS; step++) {
        volatile double vx  = x;
        volatile double vh  = h;
        volatile double vxp = vx + vh;
        int eq_plus = (vxp == vx);

        double true_d = true_deriv_d(ft, x);

        if (do_forward) {
            double num_d = (eval_d(ft, x + h) - eval_d(ft, x)) / h;
            double err = fabs(num_d - true_d);
            fprintf(data, "%s,double,%s,forward,%.*e,%.*e,%.*e,%.*e,%.*e,%d,NA\n",
                    experiment, fname,
                    DOUBLE_PREC, x, DOUBLE_PREC, h,
                    DOUBLE_PREC, num_d, DOUBLE_PREC, true_d, DOUBLE_PREC, err,
                    eq_plus);
            if (!have_fwd || err < min_err_fwd) { min_err_fwd = err; min_h_fwd = h; have_fwd = 1; }
        }
        if (do_central) {
            volatile double vxm = vx - vh;
            int eq_minus = (vxm == vx);
            double num_d = (eval_d(ft, x + h) - eval_d(ft, x - h)) / (2.0 * h);
            double err = fabs(num_d - true_d);
            fprintf(data, "%s,double,%s,central,%.*e,%.*e,%.*e,%.*e,%.*e,%d,%d\n",
                    experiment, fname,
                    DOUBLE_PREC, x, DOUBLE_PREC, h,
                    DOUBLE_PREC, num_d, DOUBLE_PREC, true_d, DOUBLE_PREC, err,
                    eq_plus, eq_minus);
            if (!have_ctr || err < min_err_ctr) { min_err_ctr = err; min_h_ctr = h; have_ctr = 1; }
        }

        double next_h = h / 10.0;
        if (next_h == 0.0 || next_h == h) break;
        h = next_h;
    }

    if (do_forward)
        fprintf(summary, "%s,double,%s,forward,%.*e,%.*e\n",
                experiment, fname, DOUBLE_PREC, min_h_fwd, DOUBLE_PREC, min_err_fwd);
    if (do_central)
        fprintf(summary, "%s,double,%s,central,%.*e,%.*e\n",
                experiment, fname, DOUBLE_PREC, min_h_ctr, DOUBLE_PREC, min_err_ctr);
}

static void run_long_double(FILE *data, FILE *summary,
                             const char *experiment, FunctionType ft, long double x,
                             int do_forward, int do_central)
{
    const char *fname = function_name(ft);
    long double h = 0.1L;

    int         have_fwd = 0, have_ctr = 0;
    long double min_err_fwd = 0.0L, min_h_fwd = 0.0L;
    long double min_err_ctr = 0.0L, min_h_ctr = 0.0L;

    for (int step = 0; step < MAX_STEPS; step++) {
        volatile long double vx  = x;
        volatile long double vh  = h;
        volatile long double vxp = vx + vh;
        int eq_plus = (vxp == vx);

        long double true_d = true_deriv_l(ft, x);

        if (do_forward) {
            long double num_d = (eval_l(ft, x + h) - eval_l(ft, x)) / h;
            long double err = fabsl(num_d - true_d);
            fprintf(data, "%s,long_double,%s,forward,%.*Le,%.*Le,%.*Le,%.*Le,%.*Le,%d,NA\n",
                    experiment, fname,
                    LDOUBLE_PREC, x, LDOUBLE_PREC, h,
                    LDOUBLE_PREC, num_d, LDOUBLE_PREC, true_d, LDOUBLE_PREC, err,
                    eq_plus);
            if (!have_fwd || err < min_err_fwd) { min_err_fwd = err; min_h_fwd = h; have_fwd = 1; }
        }
        if (do_central) {
            volatile long double vxm = vx - vh;
            int eq_minus = (vxm == vx);
            long double num_d = (eval_l(ft, x + h) - eval_l(ft, x - h)) / (2.0L * h);
            long double err = fabsl(num_d - true_d);
            fprintf(data, "%s,long_double,%s,central,%.*Le,%.*Le,%.*Le,%.*Le,%.*Le,%d,%d\n",
                    experiment, fname,
                    LDOUBLE_PREC, x, LDOUBLE_PREC, h,
                    LDOUBLE_PREC, num_d, LDOUBLE_PREC, true_d, LDOUBLE_PREC, err,
                    eq_plus, eq_minus);
            if (!have_ctr || err < min_err_ctr) { min_err_ctr = err; min_h_ctr = h; have_ctr = 1; }
        }

        long double next_h = h / 10.0L;
        if (next_h == 0.0L || next_h == h) break;
        h = next_h;
    }

    if (do_forward)
        fprintf(summary, "%s,long_double,%s,forward,%.*Le,%.*Le\n",
                experiment, fname, LDOUBLE_PREC, min_h_fwd, LDOUBLE_PREC, min_err_fwd);
    if (do_central)
        fprintf(summary, "%s,long_double,%s,central,%.*Le,%.*Le\n",
                experiment, fname, LDOUBLE_PREC, min_h_ctr, LDOUBLE_PREC, min_err_ctr);
}

int main(void)
{
    FILE *data = fopen("numeric_diff_data.csv", "w");
    FILE *summary = fopen("numeric_diff_summary.csv", "w");
    if (!data || !summary) {
        printf("CSV 파일을 열 수 없습니다.\n");
        if (data) fclose(data);
        if (summary) fclose(summary);
        return 1;
    }

    print_environment_info();

    fprintf(data,
        "experiment,type,function,method,x,h,numerical_derivative,true_derivative,"
        "absolute_error,x_plus_h_equal_x,x_minus_h_equal_x\n");
    fprintf(summary,
        "experiment,type,function,method,h_at_min_error,min_absolute_error\n");

    /* ---------------- Experiment A: h와 오차 (double, sin, x=1, 전진차분) ---------------- */
    run_double(data, summary, "A", FUNC_SIN, 1.0, /*forward*/1, /*central*/0);

    /* ---------------- Experiment B: 자료형 비교 (sin, x=1, 전진차분) ---------------- */
    run_float      (data, summary, "B", FUNC_SIN, 1.0f, 1, 0);
    run_double     (data, summary, "B", FUNC_SIN, 1.0,  1, 0);
    run_long_double(data, summary, "B", FUNC_SIN, 1.0L, 1, 0);

    /* ---------------- Experiment C: 차분법 비교 (double, sin, x=1, 전진+중심) ---------------- */
    run_double(data, summary, "C", FUNC_SIN, 1.0, /*forward*/1, /*central*/1);

    /* ---------------- Experiment D: 함수 및 x 크기 비교 (double, 전진차분) ---------------- */
    run_double(data, summary, "D", FUNC_X2,  1.0, 1, 0);
    run_double(data, summary, "D", FUNC_SIN, 1.0, 1, 0);
    run_double(data, summary, "D", FUNC_EXP, 1.0, 1, 0);
    run_double(data, summary, "D", FUNC_LOG, 1.0, 1, 0);
    /* x^2에 대한 x 크기 비교 */
    run_double(data, summary, "D", FUNC_X2, 100.0,     1, 0);
    run_double(data, summary, "D", FUNC_X2, 1000000.0, 1, 0);

    fclose(data);
    fclose(summary);

    printf("실험 완료.\n");
    printf(" - 원자료: numeric_diff_data.csv\n");
    printf(" - 요약(최소 오차): numeric_diff_summary.csv\n");

    return 0;
}
