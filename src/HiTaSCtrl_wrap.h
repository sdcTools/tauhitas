/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_tauhitas_WRAP_H_
#define SWIG_tauhitas_WRAP_H_

class SwigDirector_IProgressListener : public IProgressListener, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_IProgressListener(JNIEnv *jenv);
    virtual ~SwigDirector_IProgressListener();
    virtual void UpdateLB(int Perc);
    virtual void UpdateUB(int Perc);
    virtual void UpdateGroups(int Perc);
    virtual void UpdateTables(int Perc);
    virtual void UpdateDiscrepancy(double value);
    virtual void UpdateTime(int value);
    virtual void UpdateNSuppressed(int value);
public:
    bool swig_overrides(int n) {
      return (n < 7 ? swig_override[n] : false);
    }
protected:
    Swig::BoolArray<7> swig_override;
};

class SwigDirector_ICallback : public ICallback, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_ICallback(JNIEnv *jenv);
    virtual ~SwigDirector_ICallback();
    virtual int SetStopTime();
public:
    bool swig_overrides(int n) {
      return (n < 1 ? swig_override[n] : false);
    }
protected:
    Swig::BoolArray<1> swig_override;
};


#endif
