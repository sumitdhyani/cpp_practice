#include <string>
#include <variant>
#include <functional>
#include <iostream>

template <typename Spec, typename... Specs>
struct VariantMultiSpecProcessor : VariantMultiSpecProcessor<Specs...> {
    typedef std::function<void(const Spec&)> RequiredProcessor;
    
    VariantMultiSpecProcessor(const RequiredProcessor& requiredProcessor, const std::function<void(Specs)>&... funcs)
        : VariantMultiSpecProcessor<Specs...>(funcs...), m_requiredProcessor(requiredProcessor) {}

    void operator()(const Spec& spec) {
        m_requiredProcessor(spec);
    }

    template <typename T>
    void operator()(const T& spec) {
        VariantMultiSpecProcessor<Specs...>::operator()(spec);
    }

private:
    RequiredProcessor m_requiredProcessor;
};

template <typename Spec>
struct VariantMultiSpecProcessor<Spec> {
    typedef std::function<void(const Spec&)> RequiredProcessor;
    
    VariantMultiSpecProcessor(const RequiredProcessor& requiredProcessor)
        : m_requiredProcessor(requiredProcessor) {}

    void operator()(const Spec& var) {
        m_requiredProcessor(var);
    }

    template <typename T>
    void operator()(const T&) {};

private:
    RequiredProcessor m_requiredProcessor;
};

template <typename Specs>
struct VariantUniformProcessor{
    
};

int main() {
    typedef std::variant<int, std::string, double, char> variant;
    const variant& var1 = 2;
    const variant& var2 = "two";
    const variant& var3 = 2.5;
    const variant& var4 = '2';

    VariantMultiSpecProcessor<int, std::string, double> variantMultiSpecProcessor([](const int& num){ std::cout << num << std::endl;},
                                                [](const std::string& str){ std::cout << str.c_str() << std::endl; },
                                                [](const double& num){ std::cout << num << std::endl; });
    
    std::visit(variantMultiSpecProcessor, var1);
    std::visit(variantMultiSpecProcessor, var2);
    std::visit(variantMultiSpecProcessor, var3);
    std::visit(variantMultiSpecProcessor, var4);
    return 0;
}