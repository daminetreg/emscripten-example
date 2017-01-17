#include <iostream>
#include <functional>
#include <memory>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

class MiniXhr : public std::enable_shared_from_this<MiniXhr> {
  using val = emscripten::val;
  using url_t = std::string;

  public:

    void set_url(const url_t& url) { url_ = url; }

    void GET();

    /**
     *
     * The member function to be called from javascript.
     */
    void on_readystate(val event) {
      std::cout << "ready " << std::endl;
      std::cout << "xxhr::on_readystate: " 
          << xhr["readyState"].as<size_t>() << " - " << url_ << " :: "
          << xhr["status"].as<size_t>() << ": " 
          << xhr["statusText"].as<std::string>() << std::endl;
    }

  private:
    url_t url_;
    val xhr = val::global("XMLHttpRequest").new_();
};
   

using emscripten::class_;
EMSCRIPTEN_BINDINGS(MiniXhr) {

  /**
   * Binding for the class.
   */
  class_<MiniXhr>("MiniXhr")
    .smart_ptr<std::shared_ptr<MiniXhr>>("shared_ptr<MiniXhr>")
    .function("on_readystate", &MiniXhr::on_readystate)
    ;

  /**
   * More generic binding to bind a functor with one argument (event handler get the event)
   * Here std::function call operator from C++ is bound to function opcall() in JS.
   */
  class_<std::function<void(emscripten::val)>>("VoidValFunctor")
    .constructor<>()
    .function("opcall", &std::function<void(emscripten::val)>::operator());
   

}

/**
 *
 * Finally the interesting part : binding the member function on_readystate to the readystatechange event of XMLHttpRequest.
 *
 */

 void MiniXhr::GET() { 

  /**
   * Here this lambda could be put as function in a library, to do an JS(std::bind), 
   * it should just be overloaded for different argument count. (Im on it).
   */
  auto jsbind = [](val& target, const char* property, auto bind_expression ) {
    
    // Create an std::function from the bind expression
    std::function<void(emscripten::val)> functor = bind_expression;

    // We ensure the correct object will always be bound to the this of the function
    auto functor_adapter = val(functor)["opcall"].call<val>("bind", val(functor)); 

    // Finally we simply set the eventhandler
    target.set(property, functor_adapter);
  };

  // Here we could bind as many member function as we want.

//    jsbind(xhr, "onload", std::bind(&MiniXhr::on_load, shared_from_this(), std::placeholders::_1));
//    jsbind(xhr, "onerror", std::bind(&MiniXhr::on_error, shared_from_this(), std::placeholders::_1));
//    jsbind(xhr, "onprogress", std::bind(&MiniXhr::on_progress, shared_from_this(), std::placeholders::_1));
  jsbind(xhr, "onreadystatechange", std::bind(&MiniXhr::on_readystate, shared_from_this(), std::placeholders::_1));

  // Note that we bind with shared_from_this(), as the scope where the class was instantiated may be dead
  // and only later our callback will come back.

 xhr.call<val>("open", std::string("GET"), url_, true);
 xhr.call<val>("send");
}


int main(int argc, char** argv) {
  
  
  auto x = std::make_shared<MiniXhr>();
  x->set_url("notfound.json");
  x->GET();

  return 0;
}
