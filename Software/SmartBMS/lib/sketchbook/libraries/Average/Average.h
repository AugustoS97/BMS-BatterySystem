/*
* Copyright (c) , Majenko Technologies
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*  3. Neither the name of Majenko Technologies nor the names of its contributors may be used
*     to endorse or promote products derived from this software without
*     specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _AVERAGE_H
#define _AVERAGE_H

#if (ARDUINO >= 100)
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

#include <math.h>

inline static float sqr(float x) {
  return x*x;
}

template <class T> class Average {
private:
  // Private functions and variables here.  They can only be accessed
  // by functions within the class.
  T *_store;
  T _sum;                                               // _sum variable for faster mean calculation
  uint32_t _position;                                   // _position variable for circular buffer
  uint32_t _count;
  uint32_t _size;

public:
  // Public functions and variables.  These can be accessed from
  // outside the class.
  Average(uint32_t size);
  ~Average();
  float rolling(T entry);
  void push(T entry);
  float mean();
  T mode();
  T minimum();
  T maximum();
  float stddev();
  T get(uint32_t);
  void leastSquares(float &m, float &b, float &r);
  int getCount();
  T predict(int x);

};

template <class T> int Average<T>::getCount() {
  return _count;
}

template <class T> Average<T>::Average(uint32_t size) {
  _size = size;
  _count = 0;
  _store = (T *)malloc(sizeof(T) * size);
  _position = 0;                                            // track position for circular storage
  _sum = 0;                                                 // track sum for fast mean calculation
  for (uint32_t i = 0; i < size; i++) {
    _store[i] = 0;
  }
}

template <class T> Average<T>::~Average() {
  free(_store);
}

template <class T> void Average<T>::push(T entry) {
  if (_count < _size) {                                     // adding new values to array
    _count++;                                             // count number of values in array
  }
  else {                                                    // overwriting old values
    _sum = _sum -_store[_position];                       // remove old value from _sum
  }
  _store[_position] = entry;                                // store new value in array
  _sum += entry;                                            // add the new value to _sum
  _position += 1;                                           // increment the position counter
  if (_position >= _size) _position = 0;                    // loop the position counter
  }


  template <class T> float Average<T>::rolling(T entry) {
    push(entry);
    return mean();
  }

  template <class T> float Average<T>::mean() {
    if (_count == 0) {
      return 0;
    }
    return (float(_sum) / (float)_count);                     // mean calculation based on _sum
  }

  template <class T> T Average<T>::mode() {
    uint32_t pos;
    uint32_t inner;
    T most;
    uint32_t mostcount;
    T current;
    uint32_t currentcount;

    if (_count == 0) {
      return 0;
    }

    most = _store[0];
    mostcount = 1;
    for(pos = 0; pos < _count; pos++) {
      current = _store[pos];
      currentcount = 0;
      for(inner = pos + 1; inner < _count; inner++) {
        if(_store[inner] == current) {
          currentcount++;
        }
      }
      if(currentcount > mostcount) {
        most = current;
        mostcount = currentcount;
      }
      // If we have less array slices left than the current
      // maximum count, then there is no room left to find
      // a bigger count.  We have finished early and we can
      // go home.
      if(_count - pos < mostcount) {
        break;
      }
    }
    return most;
  }

  template <class T> T Average<T>::minimum() {
    T minval;

    if (_count == 0) {
      return 0;
    }

    minval = _store[0];

    for(uint32_t i = 0; i < _count; i++) {
      if(_store[i] < minval) {
        minval = _store[i];
      }
    }
    return minval;
  }

  template <class T> T Average<T>::maximum() {
    T maxval;

    if (_count == 0) {
      return 0;
    }

    maxval = _store[0];

    for(uint32_t i = 0; i < _count; i++) {
      if(_store[i] > maxval) {
        maxval = _store[i];
      }
    }
    return maxval;
  }

  template <class T> float Average<T>::stddev() {
    float square;
    float sum;
    float mu;
    float theta;
    int i;

    if (_count == 0) {
      return 0;
    }

    mu = mean();

    sum = 0;
    for(uint32_t i = 0; i < _count; i++) {
      theta = mu - (float)_store[i];
      square = theta * theta;
      sum += square;
    }
    return sqrt(sum/(float)_count);
  }

  template <class T> T Average<T>::get(uint32_t index) {
    if (index >= _count) {
      return 0;
    }
    int32_t cindex = _position-index;                         // position in circular buffer
    if (cindex < 0) cindex = _size + cindex;                  // need to loop around for negative cindex
      return _store[cindex];
    }

    template <class T> void Average<T>::leastSquares(float &m, float &c, float &r) {
      float   sumx = 0.0;                        /* sum of x                      */
      float   sumx2 = 0.0;                       /* sum of x**2                   */
      float   sumxy = 0.0;                       /* sum of x * y                  */
      float   sumy = 0.0;                        /* sum of y                      */
      float   sumy2 = 0.0;                       /* sum of y**2                   */

      for (int i=0;i<_count;i++)   {
        sumx  += i;
        sumx2 += sqr(i);
        sumxy += i * _store[i];
        sumy  += _store[i];
        sumy2 += sqr(_store[i]);
      }

      float denom = (_count * sumx2 - sqr(sumx));
      if (denom == 0) {
        // singular matrix. can't solve the problem.
        m = 0;
        c = 0;
        r = 0;
        return;
      }

      m = 0 - (_count * sumxy  -  sumx * sumy) / denom;
      c = (sumy * sumx2  -  sumx * sumxy) / denom;
      r = (sumxy - sumx * sumy / _count) / sqrt((sumx2 - sqr(sumx)/_count) * (sumy2 - sqr(sumy)/_count));
    }

    template <class T> T Average<T>::predict(int x) {
      float m, c, r;
      leastSquares(m, c, r); // y = mx + c;

      T y = m * x + c;
      return y;
    }

    #endif
