#include "CrazyRouters.hpp"
#include <iostream>

int main() {
	//End to end
	//auto responder = [](const std::string& str, const std::function<void(std::string, bool)> callback) {
	//	callback(getReverse(str), isPallindrome(str));
	//};
	////template<class ResponderId, class ReqId, class ReqData, class... Response>
	//EndToEndReqRespRouter<std::string, size_t, std::string, std::string, bool> reqRespRouter;
	//reqRespRouter.registerAsResponder("1", responder);
	//reqRespRouter.request("1", 1, "malayalam", [](const std::string& reversed, bool isPallindrome) {
	//	std::cout<< "The Reversed string is: " << reversed.c_str()
	//			 <<", and the string is " << (isPallindrome? "Pallindrome" : "not Pallindrome");
	//});

	

	//Pub Sub
	PubSubDataRouter<size_t, size_t, size_t, std::string> pubSubRouter;
    auto callback = [](const size_t& data, const std::string& str) {
		std::cout << "Received: " << data << ", " << str.c_str() << std::endl;
	};
	pubSubRouter.consume(1, 1, callback);
	pubSubRouter.consume(2, 1, callback);

	pubSubRouter.consume(1, 2, callback);
	pubSubRouter.consume(2, 2, callback);
    
	pubSubRouter.produce(1, 1, "A");
	pubSubRouter.produce(1, 2, "B");
	pubSubRouter.produce(2, 3, "C");
	pubSubRouter.produce(2, 4, "D");
    
	pubSubRouter.unregisterAll(2);
	pubSubRouter.unregister(2, 1);
	std::cout << "Unregistered key: 2" << std::endl;
    
    
	pubSubRouter.produce(1, 1, "A");
	pubSubRouter.produce(1, 2, "B");
	pubSubRouter.produce(2, 3, "C");
	pubSubRouter.produce(2, 4, "D");
    
	enum class ReqType{
		Square,
		Cube
	};
	
    // GenericReqRespRouter<size_t, ReqType, size_t, int, int> reqRespRouter;
	// auto squareProvider = [](const int& num, std::function<void(const int&)> callback) {
	// 	callback(num*num);
	// };

	// auto cubeProvider = [](const int& num, std::function<void(const int&)> callback) {
	// 	callback(num*num*num);
	// };

	// reqRespRouter.registerAsResponder(1, ReqType::Square, squareProvider);
	// reqRespRouter.registerAsResponder(1, ReqType::Cube, cubeProvider);

	// int base = 4;
	// reqRespRouter.request(1, ReqType::Square, base, [base](const int& res){
	// 	std::cout << "Base: " << base << ", Square: " << res << std::endl;
	// });

	// reqRespRouter.request(1, ReqType::Cube, base, [base](const int& res){
	// 	std::cout << "Base: " << base << ", Cube: " << res << std::endl;
	// });

	std::cout << "Just before end" << std::endl;
    return 0;
}