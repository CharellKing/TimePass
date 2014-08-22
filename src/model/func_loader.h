/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-06
 */

#ifndef SRC_MODEL_FUNC_LOADER_H_
#define SRC_MODEL_FUNC_LOADER_H_

namespace TimePass {
template<typename Obj, typename Param>
class FuncLoader {
 public:
  typedef void (Obj::*FUNC_PTR)(Param* p_param);

  FuncLoader(Obj* p_obj, FUNC_PTR p_func, Param* p_param)
      : p_obj_(p_obj),
        p_func_(p_func),
        p_param_(p_param) {
  }

  void CallFunc() {
    (p_obj_->*p_func_)(p_param_);
  }

 private:
  Obj* p_obj_;
  FUNC_PTR p_func_;
  Param* p_param_;
};
};  // namespace TimePass

#endif  // SRC_MODEL_FUNC_LOADER_H_
