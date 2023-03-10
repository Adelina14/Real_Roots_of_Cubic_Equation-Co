/*
  Created by Mamatova Adelina
  adelina.m14@mail.ru
___________________________
  Real Roots of Cubic Equation (Tomas Co, 10/7/2014)
  https://studylib.net/doc/18485147/real-roots-of-cubic-equation
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <math.h>
#include <stdexcept>
#include <limits>
//#include <complex.h>
#include "excerpt.h"
#define MAX_DISTANCE 10e-5
using namespace std;

//#define DEBUG //раскомментировать для вывода

/*template<typename fp_t>
auto diff_of_products(fp_t a, fp_t b, fp_t c, fp_t d) //fms
{
    auto w = d * c;
    auto e = fmaf (c, -d, w);
    auto f = fmaf (a, b, -w);
    return f + e;
}*/

template<typename fp_t>
int solve_cubic(std::vector<fp_t> coefficients, std::vector<fp_t> &roots) {

    fp_t a, b, c, d, phi, A, B, D, p, q, pp;
    fp_t pi=static_cast<fp_t>(numbers::pi);
    fp_t eps=static_cast<fp_t>(std::numeric_limits<fp_t>::epsilon());
    fp_t twoPi=2*pi;
    //Coefficients
    a = coefficients[3];
    b = coefficients[2];
    c = coefficients[1];
    d = coefficients[0];
    int cnt_real_roots = 0;
    fp_t twoNinths=static_cast<fp_t>(2.0L/9);
    fp_t oneThird=static_cast<fp_t>(1.0L/3);
    fp_t oneFourth=static_cast<fp_t>(1.0L/4);
    fp_t oneTwentySeventh=static_cast<fp_t>(1.0L/27);


    if (a != 0){

        b = b/a;
        c = c/a;
        d = d/a;
        fp_t bThird=b*oneThird;

        //подход Чирнхауса-Виета: пусть x=t+B
        //t=x-b/3 (B=-b/3a) -> коэф.при t^2 = 0 -> получаем t^3+pt+q=0

        //p = c - b*b/3;
         p = fma<fp_t>(-bThird,b,c);
        //q = b*(2*b*b/9 - c)/3+d;
        q=fma<fp_t>(bThird, fma(b*twoNinths,b,-c),d);
        //D=p*p*p/27+q*q/4;
        pp=p*p;
        D=fma<fp_t>(p*oneTwentySeventh,pp,0)+fma<fp_t>(q*oneFourth,q,0);
        A = 2*sqrt(abs(p)*oneThird), phi;
#ifdef DEBUG
        cout<< "cubic::\n";
        cout <<a<<"x^3+"<<b<<"x^2+"<<c<<"x+"<<d<< '\n';
        //cout<< "D=" << D << " A=" << A  << '\n';
        //cout<< "p=" << p << " q=" << q << '\n';
#endif
        if (D <= 0 || abs(D) < eps){ // 3 действит.корня;  1 корень при (p=q=0)

            if (p == 0 && q == 0){
                roots[0]=(-bThird);
                cnt_real_roots = 1;

            } else {
                fp_t arg = 3*q/(A*p);
                cnt_real_roots = 3;
#ifdef DEBUG
            //cout<< "arg=" << arg << '\n';
#endif
            phi = (abs(arg) > 1 ? 0 : acos(arg));
#ifdef DEBUG
            roots[0]=fma(A,cos((phi*oneThird)),-bThird);
            roots[1]=fma(A,cos((phi + twoPi)*oneThird),-bThird);
            roots[2]=fma(A,cos((phi + twoPi*2)*oneThird),-bThird);
           // cout<<roots[0]<<" "<<roots[1]<<" "<<roots[2]<<"\n";
#else
            roots[0]=fma(A,cos((phi*oneThird)),-bThird);              //переходим обратно к x=t+B=Acos(phi)+B
            roots[1]=fma(A,cos((phi + twoPi)*oneThird),-bThird);
            roots[2]=fma(A,cos((phi + twoPi*2)*oneThird),-bThird);
#endif
            }
        } else {
            // D > 0, только действит.корни
            cnt_real_roots = 1;
            if (p < 0){
                phi = acosh(-3*abs(q)/(A*p));
                roots[0]=((q > 0 ? -A : A)*cosh(phi*oneThird) - bThird);

            } else if (p == 0) {
                roots[0]=(cbrt(q) - bThird);
            } else {  // p > 0
                phi = asinh(3*q/(A*p));
                //roots[0]=(-A*sinh(phi/3) - b/3);
                roots[0]=fma(-A,sinh(phi*oneThird),-bThird);
            }
        }
    }
#ifdef DEBUG
    for (int i=0;i<cnt_real_roots;i++)
        cout<<"roots["<<i<<"] is "<<roots[i]<<"\n";
#endif
    return cnt_real_roots;
}

// Для вещественной арифметики
template<typename fp_t>
pair<fp_t, fp_t> testPolynomial(unsigned int roots_count) {
    fp_t max_absolute_error, max_relative_error; // макс.абсолют. и относит. погрешности
    vector<fp_t> roots_computed(roots_count); // найденные корни методом
    vector<fp_t> roots(roots_count), coefficients(roots_count + 1); // истинные корни; коэффициенты
    generate_polynomial<fp_t>(roots_count, 0, roots_count, 0, MAX_DISTANCE, -1, 1, roots,
                              coefficients);
    int cnt_real_roots = solve_cubic(coefficients, roots_computed); // находим число вещ.корней и сами корни
    if (cnt_real_roots != 0 && cnt_real_roots != -1) { // если корни найдены и мы не попали в какой-то исключ.случай,
        // то сравниваем найденные корни с истинными
        compare_roots<fp_t>(roots_computed.size(), roots.size(), roots_computed, roots,
                            max_absolute_error, max_relative_error);

    } else max_absolute_error = 0, max_relative_error = 0;
    return pair<fp_t, fp_t>(max_absolute_error, max_relative_error);
}


template<typename fp_t>
fp_t testPolynomial_complex(unsigned int roots_count) {
    fp_t max_absolute_error, max_relative_error;
    std::vector<std::complex<fp_t>> roots_computed(roots_count);//вектор компл. корней
    vector<fp_t> roots(roots_count), roots_computed_real(roots_count); //вектор вещ.корней
    vector<fp_t> coefficients(roots_count + 1);
    generate_polynomial<fp_t>(roots_count, 0, roots_count, 0, numeric_limits<fp_t>::min(), -1, 1, roots, coefficients);
    roots_computed = solve_cubic_complex(coefficients, roots_computed); //находим корни - комплексный вектор

    if (!roots_computed.empty()) // если корни найдены
    {
        roots_computed_real = return_real_roots(roots_computed); //проверяем на комплексность - > отбрасываем
        // получаем вектор вещественных корней
        if (!roots_computed_real.empty()) { //если корни действительные - сравниваем

            compare_roots<fp_t>(roots_computed_real.size(), roots.size(), roots_computed_real, roots,
                                max_absolute_error, max_relative_error);
            return max_absolute_error;
        }
        else return std::numeric_limits<fp_t>::infinity(); //если корни все же комплексные
    }
    else return std::numeric_limits<fp_t>::infinity();
}
typedef float fp_t;
int main() {
    // МЕТОД С ВЕЩЕСТВЕННЫМИ ВЫЧИСЛЕНИЯМИ
    float max_absolut_deviation = 0;
    float max_relative_deviation = 0;
    for (auto i = 0; i < 10'000'000; ++i) {
        auto deviation = testPolynomial<float>(3);

        if (deviation.first > max_absolut_deviation) {
            max_absolut_deviation = deviation.first;
        }
        if (deviation.second > max_relative_deviation) {
            max_relative_deviation = deviation.second;
        }
    }
    cout << endl << "MAX_ABSOLUT_deviation = " << max_absolut_deviation << endl;
    cout << endl << "MAX_RELATIVE_deviation = " << max_relative_deviation << endl;
}
