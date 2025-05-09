#include "CrazyRouters.hpp"
#include <string>
#include <set>
#include <variant>
#include <iostream>

struct PriceSubMsg {
    std::string exchange;
    std::string key;
};

struct PriceUnsubMsg {
    std::string exchange;
    std::string key;
};

struct SettlementSubMsg {
    std::string exchange;
    std::string key;
};

struct SettlementUnsubMsg {
    std::string exchange;
    std::string key;    
};

struct PriceData {
    std::string key;
};

struct SettlementData {
    std::string key;
};

typedef std::variant<SettlementData, PriceData, std::string> AppData;

typedef PubSubDataRouter<std::string, size_t, AppData> AppPubsubRouter;
typedef std::shared_ptr<AppPubsubRouter> AppPubsubRouter_SPtr;

enum class ClientEnquiryType {
    NumPriceUpdatesTillNow,
    NumSettlementUpdatesTillNow,
    TotalNumSubscriptions
};

typedef EndToEndReqRespRouter<std::string, std::string, ClientEnquiryType, size_t> AppReqResponseRouter;
typedef std::shared_ptr<AppReqResponseRouter> AppReqResponseRouter_SPtr;

class PriceProvider {
    struct Visitor{
        void operator()(PriceSubMsg const& msg) {
            std::cout << "PriceSubscription for: " << msg.key.c_str() << std::endl;
        }

        void operator()(PriceUnsubMsg const& msg) {
            std::cout << "PriceSubscription for: " << msg.key.c_str() << std::endl;
        }

        void operator()(SettlementSubMsg const& msg) {
            std::cout << "SettlementSubMsg for: " << msg.key.c_str() << std::endl;
        }

        void operator()(SettlementUnsubMsg const& msg) {
            std::cout << "SettlementUnsubMsg for: " << msg.key.c_str() << std::endl;
        }

        template <typename T>
        void operator()(T const&) {}
    };

    PriceProvider(const std::string& exchange, const AppPubsubRouter_SPtr& appRouter) : m_appRouter(appRouter) {
        m_appRouter->consume(exchange, (size_t)this, [this](const AppData& appdata) {
            std::visit(Visitor(), appdata);
        });
    }

    ~PriceProvider() {
        m_appRouter->unregisterAll((size_t)this);
    }
    private:
    AppPubsubRouter_SPtr m_appRouter;
};

class PriceClient {
    public:

    PriceClient(const AppPubsubRouter_SPtr& appPubsubRouter,
                const AppReqResponseRouter_SPtr& appReqResponseRouter,
                std::string clientId) : 
        m_appPubsubRouter(appPubsubRouter),
        m_appReqResponseRouter(appReqResponseRouter)
    {
        m_appReqResponseRouter->registerAsResponder(std::to_string((size_t)this), [this](const ClientEnquiryType& clientEnquiryType, const std::function<void(size_t)>& callback) {
            onStatsEnquiry(clientEnquiryType, callback);
        });
        std::string objId = std::to_string((size_t)this);
        m_appPubsubRouter->produce(objId, objId);
    }

    void onStatsEnquiry(const ClientEnquiryType& clientEnquiryType, const std::function<void(size_t)>& callback) {
        switch(clientEnquiryType) {
            case ClientEnquiryType::NumPriceUpdatesTillNow:
                callback(m_numPriceUpdatesTillNow);
                break;
            case ClientEnquiryType::NumSettlementUpdatesTillNow:
                callback(m_numSettlementUpdatesTillNow);
                break;
            case ClientEnquiryType::TotalNumSubscriptions:
                callback(m_numSubscriptions);
                break;
            default:
                break;
        }
	}

    void onPrice(const PriceData& priceData) {
        std::cout << "Price Received: " << priceData.key.c_str() << std::endl;
    }

    void onSettlement(const SettlementData& settlementData) {
        std::cout << "Settlement Received: " << settlementData.key.c_str() << std::endl;
    }

    void onPriceSubMsg(PriceSubMsg const& msg) {
        std::cout << "PriceSubscription for: " << msg.key.c_str() << std::endl;
    }

    void onPriceUnsubMsg(PriceUnsubMsg const& msg) {
        std::cout << "PriceUnsubscription for: " << msg.key.c_str() << std::endl;
    }

    void onSettlementSubMsg(SettlementSubMsg const& msg) {
        std::cout << "SettlementSubMsg for: " << msg.key.c_str() << std::endl;
    }

    void onSettlementUnsubMsg(SettlementUnsubMsg const& msg) {
        std::cout << "SettlementUnsubMsg for: " << msg.key.c_str() << std::endl;
    }
    private:
    AppPubsubRouter_SPtr m_appPubsubRouter;
    AppReqResponseRouter_SPtr m_appReqResponseRouter;
    std::string m_clientId;
    
    size_t m_numPriceUpdatesTillNow;
    size_t m_numSettlementUpdatesTillNow;
    size_t m_numSubscriptions;
};

struct Admin {
    static int reqId;
    Admin(const AppReqResponseRouter_SPtr& appReqResponseRouter) : m_appReqResponseRouter(appReqResponseRouter) {}
    
    int getClientDetails(const ClientEnquiryType& clientEnquiryType, const std::string& clientId, const std::function<void(const size_t&)>& callback) {
        m_appReqResponseRouter->request(clientId, std::to_string(reqId++), clientEnquiryType, [callback](const size_t& res) { callback(res); });
    }

    private:
    AppReqResponseRouter_SPtr m_appReqResponseRouter;
};

int Admin::reqId = 0;


int main() {
	auto responder = [](const std::string& str, const std::function<void(std::string, bool)> callback) {
		callback(getReverse(str), isPallindrome(str));
	};
	//template<class ResponderId, class ReqId, class ReqData, class... Response>
	EndToEndReqRespRouter<std::string, size_t, std::string, std::string, bool> reqRespRouter;
	reqRespRouter.registerAsResponder("1", responder);
	reqRespRouter.request("1", 1, "malayalam", [](const std::string& reversed, bool isPallindrome) {
		std::cout<< "The Reversed string is: " << reversed.c_str()
				 <<", and the string is " << (isPallindrome? "Pallindrome" : "not Pallindrome");
	});

	//PubSubDataRouter<size_t, size_t, size_t, std::string> pubSubRouter;
	auto callback = [](const size_t& data, const std::string& str) {
		std::cout << "Received: " << data << ", " << str.c_str() << std::endl;
	};

//
	//pubSubRouter.consume(1, 1, callback);
	//pubSubRouter.consume(2, 1, callback);
//
	//pubSubRouter.produce(1, 1, "A");
	//pubSubRouter.produce(1, 2, "B");
	//pubSubRouter.produce(2, 3, "C");
	//pubSubRouter.produce(2, 4, "D");
//
	//pubSubRouter.unregister(2, 1);
	//std::cout << "Unregistered key: 2" << std::endl;
//
//
	//pubSubRouter.produce(1, 1, "A");
	//pubSubRouter.produce(1, 2, "B");
	//pubSubRouter.produce(2, 3, "C");
	//pubSubRouter.produce(2, 4, "D");
//
//
	//std::cout << "Just before end" << std::endl;
    return 0;
}