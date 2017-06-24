// following code might be needed in some case
// #define __STDC_LIMIT_MACROS
// #include <stdint.h>
#include <jsapi.h>
#include <js/Initialization.h>

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>

JSContext *cx = nullptr;

/* The class of the global object. */
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    JS_GlobalObjectTraceHook
};

//Generic case.
template<size_t argIndex, typename ...A>
class FunctionParamValidator
{
public:
	static bool isValid(JS::CallArgs args)
	{
		std::cout << "Reached Generic Case" << std::endl;
		return true;
	}
};

//Specialization
template<size_t argIndex, typename T, typename ...A>
class FunctionParamValidator<argIndex, T, A...>
{
public:
	static bool isValid(JS::CallArgs args)
	{
		//std::cout << "Arg Index: " << argIndex << ", Value: " << arguments[argIndex] << ", Vector Size: " << arguments.size() << std::endl;

		if(argIndex == args.length())
		{
			std::cout << " - Invalid - Overflow" << std::endl;
			return false;
		}

		std::cout << " - Delegating..." << std::endl;
		
		return FunctionParamValidator<argIndex + 1, A...>::isValid(args);
	}
};

template <typename T>
T convertToNative(JS::HandleValue val)
{
	T nativeValue;
	return nativeValue;
}

template <>
int32_t convertToNative<int32_t>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
int64_t convertToNative<int64_t>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
uint64_t convertToNative<uint64_t>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
uint32_t convertToNative<uint32_t>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
uint16_t convertToNative<uint16_t>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
double convertToNative<double>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
float convertToNative<float>(JS::HandleValue val)
{
	return val.toInt32();
}

template <>
std::string convertToNative<std::string>(JS::HandleValue val)
{
	return JS_EncodeString(cx, val.toString());
}


template <typename T>
JS::HandleValue convertFromNative(const T &t)
{
	return JS::HandleValue();
}

template <>
JS::HandleValue convertFromNative(const int64_t &t)
{
	return JS::RootedValue(cx, JS_NumberValue(t));
}

template <>
JS::HandleValue convertFromNative(const int32_t &t)
{
	return JS::RootedValue(cx, JS_NumberValue(t));
}

template <>
JS::HandleValue convertFromNative(const uint64_t &t)
{
	return JS::RootedValue(cx, JS_NumberValue(t));
}

template <>
JS::HandleValue convertFromNative(const uint32_t &t)
{
	return JS::RootedValue(cx, JS_NumberValue(t));
}

template <>
JS::HandleValue convertFromNative(const uint16_t &t)
{
	return JS::RootedValue(cx, JS_NumberValue(t));
}

template <>
JS::HandleValue convertFromNative(const std::string &t)
{
	JSString *jsString = JS_NewStringCopyN(cx, t.c_str(), t.size());

	JS::Value returnValue;

	returnValue.setString(jsString);

	return JS::RootedValue(cx, returnValue);
}

template <typename T>
T convertToNative(const size_t argIndex, JS::CallArgs args)
{
	T t;

	if(argIndex >= args.length())
		return t;
	
	JS::HandleValue val = args[argIndex];

	return convertToNative<T>(val);
}

template <typename... Args>
std::tuple<Args...> callArgsToTuple(JS::CallArgs args)
{
  size_t argIndex = 0;
  size_t argSize = sizeof...(Args);
  return std::make_tuple(convertToNative<Args>(argSize-1-argIndex++, args)...);
}

// implementation details, users never invoke these directly
namespace detail
{
    template <typename R, typename F, typename Tuple, bool Done, int Total, int... N>
    struct call_impl
    {
        static R call(F f, Tuple && t)
        {
            return call_impl<R, F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
        }
    };

    template <typename R, typename F, typename Tuple, int Total, int... N>
    struct call_impl<R, F, Tuple, true, Total, N...>
    {
        static R call(F f, Tuple && t)
        {
            return f(std::get<N>(std::forward<Tuple>(t))...);
        }
    };
}

// user invokes this
template <typename R, typename F, typename Tuple>
R call(F f, Tuple && t)
{
    typedef typename std::decay<Tuple>::type ttype;
    return detail::call_impl<R, F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(f, std::forward<Tuple>(t));
}


template<typename R, typename ...A>
JS::HandleValue runNative(const std::function<R(A...)> &callback, JS::CallArgs args)
{
	return convertFromNative(call<R>(callback, callArgsToTuple<A...>(args)));
}

template<typename R=void, typename ...A>
JS::HandleValue runNative(const std::function<void(A...)> &callback, JS::CallArgs args)
{
	call<void>(callback, callArgsToTuple<A...>(args));
	return JS::RootedValue(cx);
}


template<typename R, typename ...A>
class KinslayerJSFunctionBase
{
public:
	std::function<R(A...)> callback;

	KinslayerJSFunctionBase(std::function<R(A...)> callback)
	{
		this->callback = callback;
	}

	JS::HandleValue process(JS::CallArgs args)
	{
		std::tuple<A...> t = callArgsToTuple<A...>(args);

		for(size_t index = 0;index < sizeof...(A);++index)
		{
			//std::cout << "TUPLE[" << index << "]: `" << std::get<
		}

		//call(callback, callArgsToTuple<A...>(args));
		return runNative<R, A...>(callback, args);

		//bool argumentsAreValid = FunctionParamValidator<0, A...>::isValid(args);
		
		//std::cout << "NUMBER OF FUNCTION ARGS: " << sizeof...(A) << ", Arguments are valid: " << (argumentsAreValid ? "YES" : "NO") << std::endl;
	}
};

std::map<std::string, void *> jsFunctionMap;

template<typename R, typename ...T>
bool callNativeFunction(JSContext *cx, unsigned argc, JS::Value *vp)
{
	//std::cout << "In callNativeFunction..." << std::endl;
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

	JSObject &callee = args.callee();
	args.thisv();

	if(JS_ObjectIsFunction(cx, &callee))
	{
		std::cout << "Callee Is Function." << std::endl;
	}

	JS::RootedValue calleeValue(cx, JS::ObjectValue(callee));
	JSFunction *jsFunction = JS_ValueToFunction(cx, calleeValue);

	//std::cout << "[CALLBACK] Function Ptr: " << jsFunction << std::endl;

	auto chars = JS_EncodeString(cx, JS_GetFunctionId(jsFunction));
	//std::cout << "[CALLBACK] Function ID String: " << chars << std::endl;

	std::string functionIdCppString = std::string(chars);

	auto functionBase = (KinslayerJSFunctionBase<R, T...>*)jsFunctionMap[functionIdCppString];
	
	JS::HandleValue returnValue = functionBase->process(args);

	args.rval().set(returnValue);

	return true;
}

template <typename R, typename ...T>
JSObject* createNativeFunction(JS::Handle<JSObject*> parentObject, const std::string &functionName, const std::function<R(T...)> &fn)
{
	auto functionBase = new KinslayerJSFunctionBase<R, T...>(fn);
	/**
	JSClass classDef =
	{
	    "FunctionParent",
		JSCLASS_HAS_PRIVATE,
		nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr
	};

	JSObject *functionParentObject = JS_NewObject(cx, &classDef);
	**/
	JSFunction *jsFunction = JS_DefineFunction(
		cx,
		parentObject,
		functionName.c_str(),
		callNativeFunction<R, T...>,
		sizeof...(T),
		0
	);

	if(!jsFunction)
		std::cout << "Failed to create function." << std::endl;

	JSObject *functionObject = JS_GetFunctionObject(jsFunction);

	//std::cout << "[SETUP] Function Ptr: " << jsFunction << std::endl;

	JSString *functionIdString = JS_GetFunctionId(jsFunction);
	auto chars = JS_EncodeString(cx, functionIdString);
	//std::cout << "[SETUP] Function ID String: " << chars << std::endl;

	std::string functionIdCppString = std::string(chars);
	jsFunctionMap[functionIdCppString] = functionBase;

	return functionObject;
}

template <typename R, typename ...T>
void createNativeFunction(JS::Handle<JSObject*> parentObject, const std::string &functionName, R (*fn)(T...))
{
	std::function<R(T...)> cppFn = fn;
	createNativeFunction(parentObject, functionName, cppFn);
}

int addTwo(int a, int b)
{
	return a + b;
}

int addThree(int a, int b, int c)
{
	return a + b + c;
}

void printAwkward(std::string str, int i)
{
	std::cout << "Str: " << str << ", I: " << i << std::endl;
}

std::string concat(std::string arg1, const std::string arg2)
{
	return arg1 + arg2;
}

void print(std::string message)
{
	std::cout << "[PRINT]: " << message << std::endl;
}


/*** Native Class ***/

template<typename C>
void nativeClassDestructor(JSFreeOp *freeOp, JSObject *obj)
{
	std::cout << "Printer Finalize..." << std::endl;
	C *c = (C *) JS_GetPrivate(obj);
	delete c;
}

template <typename C, typename R, R (C::*fnPtr)(void)>
bool callNativeGetter(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	C *c = (C *) JS_GetPrivate(&args.thisv().toObject());

	R result = (c->*fnPtr)();

	return true;
}

template <typename C, typename T, void (C::*fnPtr)(const T &)>
bool callNativeSetter(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	C *c = (C *) JS_GetPrivate(&args.thisv().toObject());

	T t;
	(c->*fnPtr)(t);

	return true;
}

template<typename C>
class NativeClass
{
public:
	static const JSClass* getClassDefinition()
	{
		static constexpr JSClass classDef =
		{
		    C::getClassName(),
			JSCLASS_HAS_PRIVATE,
			nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, &nativeClassDestructor<C>
		};

		return &classDef;
	}
};

template<typename R, typename ...T>
void acceptFunction(const std::function<R(T...)> &fn)
{

}

class MyPrinter : public NativeClass<MyPrinter>
{
	private:
		std::string name;
		std::string title;
	public:
		std::string getName()
		{
			return name;
		}

		void setName(const std::string &name)
		{
			this->name = name;
		}

		std::string getTitle()
		{
			return title;
		}

		void setTitle(const std::string &title)
		{
			this->title = title;
		}

		void test()
		{
			std::function<std::string(MyPrinter*)> fn = &MyPrinter::getName;
		}

		static void testStatic() {}

		static JSObject* createPrototype(JSContext *cx)
		{
			JSObject *jsObject = JS_NewObject(cx, nullptr);

			JS::Handle<JSObject*> handleObject = JS::Rooted<JSObject*>(cx, jsObject);

			acceptFunction(&MyPrinter::getTitle);

			//createNativeFunction(handleObject, "setTitle", &MyPrinter::setTitle);
			//JS_SetProperty(cx, jsObject, "setTitle", createNativeFunction(jsObject, "setTitle"))


			return jsObject;
		}

		static const JSPropertySpec *getProperties()
		{
			static constexpr JSPropertySpec propertySpec[] = {
				JS_PSG("name", (callNativeGetter<MyPrinter, std::string, &MyPrinter::getName>), 0),
				JS_PSGS("title", (callNativeGetter<MyPrinter, std::string, &MyPrinter::getTitle>), (callNativeSetter<MyPrinter, std::string, &MyPrinter::setTitle>), 0)
			};

			return propertySpec;
		}

		static constexpr const char *getClassName()
		{
			return "Printer";
		}

		//Static mapper on the class, key=function/property name, value=void* pointer to method?
};

template<typename C>
bool nativeClassConstructor(JSContext *cx, unsigned argc, JS::Value *vp)
{
	std::cout << "Native Class Constructor..." << std::endl;
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JSObject *obj = JS_NewObjectForConstructor(cx, C::getClassDefinition(), args);

    if (!obj)
        return false;
	
    C *c = new C;
    
	if (c == nullptr)
	{
        JS_ReportOutOfMemory(cx);
        return false;
    }

    JS_SetPrivate(obj, c);
    args.rval().setObject(*obj);
    return true;
}

template<typename C>
void initClass(JSContext *cx, JS::Handle<JSObject*> global)
{
	JSObject *obj = JS_InitClass(
		cx,
		global,
		nullptr,
		C::getClassDefinition(),
		nativeClassConstructor<C>,
		1,
		C::getProperties(),
		NULL,
		NULL,
		NULL
	);
}

int addFn(int a, int b)
{
	return a+b;
}

std::string subFn(int a, int b)
{
	return std::string("abcdef");
}

std::string subFn1(int a)
{
	return std::string("abcdef");
}

std::string subFn0()
{
	return std::string("abcdef");
}


template <typename R, typename ...T>
void callFnMain(int a, int b, R (*fnType)(T...))
{
	std::cout << "In here." << std::endl;
}




template <typename R, R (*fnType)()>
void callFn(int a, int b)
{
	callFnMain<R>(a, b, fnType);
}

template <typename R, typename T1, R (*fnType)(T1)>
void callFn(int a, int b)
{
	callFnMain<R, T1>(a, b, fnType);
}

template <typename R, typename T1, typename T2, R (*fnType)(T1, T2)>
void callFn(int a, int b)
{
	callFnMain<R, T1, T2>(a, b, fnType);
}

template <typename R, typename T1, typename T2, typename T3, R (*fnType)(T1, T2, T3)>
void callFn(int a, int b)
{
	callFnMain<R, T1, T2, T3>(a, b, fnType);
}



template<typename T, T> struct C;

template<typename T, typename R, typename ...Args, R (T::*F)(Args...)>
struct C<R (T::*)(Args...), F> {

  R operator()(T &obj, Args &&... args) {
    return (obj.*F)(std::forward<Args>(args)...);
  }

};

struct A {
  int foo(int i) { return i; }
};

int main(int argc, const char *argv[])
{

  C<int(A::*)(int), &A::foo> c;
  A a;
  std::cout << c(a, 42) << std::endl;

	callFn<int, int, int, &addFn>;
	callFn<std::string, int, int, &subFn>;
	callFn<std::string, int, &subFn1>;
	callFn<std::string, &subFn0>;


	//std::string (MyPrinter::*fnPtr)(void) = &MyPrinter::getName;

	//JSNative n = (callNativeGetterSpecific<MyPrinter, &MyPrinter::getName>);

	JS_Init();

    JSRuntime *rt = JS_NewRuntime(8L * 1024 * 1024);
    if (!rt)
        return 1;
	
    cx = JS_NewContext(rt, 8192);
    if (!cx)
        return 1;
	
    { // Scope for our various stack objects (JSAutoRequest, RootedObject), so they all go
      // out of scope before we JS_DestroyContext.

      JSAutoRequest ar(cx); // In practice, you would want to exit this any
                            // time you're spinning the event loop

      JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class, nullptr, JS::FireOnNewGlobalHook));
      if (!global)
          return 1;

      JS::RootedValue rval(cx);

      { // Scope for JSAutoCompartment
        JSAutoCompartment ac(cx, global);
        JS_InitStandardClasses(cx, global);

//		if (!JS_DefineFunctions(cx, global, myjs_global_functions))
//				return false;

		std::cout << "Creating native functions..." << std::endl;
		createNativeFunction(global, "addTwo", addTwo);
		createNativeFunction(global, "addThree", addThree);
		createNativeFunction(global, "concat", concat);
		createNativeFunction(global, "print", print);
		createNativeFunction(global, "printAwkward", printAwkward);

		std::cout << "Initializing native classes..." << std::endl;
		initClass<MyPrinter>(cx, global);

		//std::cout << "Native Class Result: " << obj << std::endl;

		JS::Value myprop;
		//JS::Rooted<JS::Value> myPropRooted = JS::Rooted<JS::Value>(myprop);
		//JS::HandleValue mypropHandle = JS::HandleValue(myprop);
//		JS_SetProperty(cx, global, "myprop", myprop);

        //const char *script = "var r = addTwo(4,5); print('hi there: ' + r + ': ' + concat('str1', 'str2')); printAwkward('str', 3);";

		std::cout << "Compiling & executing script..." << std::endl;
		const char *script = "var p = new Printer(); print('Printer: ' + p);";
        const char *filename = "noname";
        int lineno = 1;
        JS::CompileOptions opts(cx);
        opts.setFileAndLine(filename, lineno);

		std::cout << "Evaluating..." << std::endl;
        bool ok = JS::Evaluate(cx, opts, script, strlen(script), &rval);

		std::cout << "Evaluation result: " << ok << std::endl;
        if (!ok)
		{
			std::cout << "Not Okay." << std::endl;
          return 1;
		}
      }

      //JSString *str = rval.toString();
      //printf("%s\n", JS_EncodeString(cx, str));
    }

    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
    return 0;
}