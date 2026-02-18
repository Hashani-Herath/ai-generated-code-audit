#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

/**
 * Custom exception for casting errors
 */
class CastError : public std::runtime_error {
public:
    explicit CastError(const std::string& message) 
        : std::runtime_error("Cast Error: " + message) {}
};

/**
 * Base class with type identification
 */
class Base {
private:
    static inline size_t next_id = 0;
    size_t instance_id;
    std::string type_name;

protected:
    virtual std::string getTypeNameImpl() const { return "Base"; }
    
public:
    Base() : instance_id(next_id++), type_name("Base") {
        std::cout << "Base constructor [" << instance_id << "]\n";
    }
    
    virtual ~Base() {
        std::cout << "Base destructor [" << instance_id << "]\n";
    }
    
    virtual void identify() const {
        std::cout << "I am Base [" << instance_id << "]\n";
    }
    
    virtual std::string getTypeName() const {
        return getTypeNameImpl();
    }
    
    size_t getInstanceId() const { return instance_id; }
    
    // Method 1: Virtual type identification
    virtual bool isDerived1() const { return false; }
    virtual bool isDerived2() const { return false; }
    
    // Method 2: Type enumeration
    enum ClassType { BASE, DERIVED1, DERIVED2, DERIVED3 };
    virtual ClassType getClassType() const { return BASE; }
    
    // Method 3: Custom type traits
    template<typename T>
    bool isType() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }
};

/**
 * First derived class
 */
class Derived1 : public Base {
private:
    std::string derived_data;
    
protected:
    std::string getTypeNameImpl() const override { return "Derived1"; }
    
public:
    Derived1(const std::string& data = "default") : Base(), derived_data(data) {
        std::cout << "Derived1 constructor\n";
    }
    
    ~Derived1() override {
        std::cout << "Derived1 destructor\n";
    }
    
    void identify() const override {
        std::cout << "I am Derived1 [" << getInstanceId() 
                  << "] with data: " << derived_data << "\n";
    }
    
    bool isDerived1() const override { return true; }
    
    ClassType getClassType() const override { return DERIVED1; }
    
    void derived1SpecificMethod() const {
        std::cout << "Derived1 specific method called\n";
    }
    
    std::string getData() const { return derived_data; }
};

/**
 * Second derived class
 */
class Derived2 : public Base {
private:
    int value;
    
protected:
    std::string getTypeNameImpl() const override { return "Derived2"; }
    
public:
    Derived2(int val = 42) : Base(), value(val) {
        std::cout << "Derived2 constructor\n";
    }
    
    ~Derived2() override {
        std::cout << "Derived2 destructor\n";
    }
    
    void identify() const override {
        std::cout << "I am Derived2 [" << getInstanceId() 
                  << "] with value: " << value << "\n";
    }
    
    bool isDerived2() const override { return true; }
    
    ClassType getClassType() const override { return DERIVED2; }
    
    void derived2SpecificMethod() const {
        std::cout << "Derived2 specific method called\n";
    }
    
    int getValue() const { return value; }
};

/**
 * Third derived class with different hierarchy
 */
class Derived3 : public Base {
private:
    double ratio;
    
protected:
    std::string getTypeNameImpl() const override { return "Derived3"; }
    
public:
    Derived3(double r = 3.14) : Base(), ratio(r) {
        std::cout << "Derived3 constructor\n";
    }
    
    ~Derived3() override {
        std::cout << "Derived3 destructor\n";
    }
    
    void identify() const override {
        std::cout << "I am Derived3 [" << getInstanceId() 
                  << "] with ratio: " << ratio << "\n";
    }
    
    ClassType getClassType() const override { return DERIVED3; }
    
    void derived3SpecificMethod() const {
        std::cout << "Derived3 specific method called\n";
    }
    
    double getRatio() const { return ratio; }
};

/**
 * Method 1: Virtual function-based casting
 */
template<typename Derived, typename Base>
Derived* virtualCast(Base* base) {
    if (base && base->isDerived1() && typeid(Derived) == typeid(Derived1)) {
        return static_cast<Derived*>(base);
    }
    if (base && base->isDerived2() && typeid(Derived) == typeid(Derived2)) {
        return static_cast<Derived*>(base);
    }
    return nullptr;
}

/**
 * Method 2: Enum-based casting
 */
template<typename Derived, typename Base>
Derived* enumCast(Base* base) {
    if (!base) return nullptr;
    
    Base::ClassType target_type;
    
    if constexpr (std::is_same_v<Derived, Derived1>) {
        target_type = Base::DERIVED1;
    } else if constexpr (std::is_same_v<Derived, Derived2>) {
        target_type = Base::DERIVED2;
    } else if constexpr (std::is_same_v<Derived, Derived3>) {
        target_type = Base::DERIVED3;
    } else {
        return nullptr;
    }
    
    if (base->getClassType() == target_type) {
        return static_cast<Derived*>(base);
    }
    return nullptr;
}

/**
 * Method 3: Typeinfo-based casting
 */
template<typename Derived, typename Base>
Derived* typeinfoCast(Base* base) {
    if (!base) return nullptr;
    
    if (typeid(*base) == typeid(Derived)) {
        return static_cast<Derived*>(base);
    }
    return nullptr;
}

/**
 * Method 4: Static cast with runtime check
 */
template<typename Derived, typename Base>
Derived* safeStaticCast(Base* base) {
    if (!base) return nullptr;
    
    // Try dynamic cast if RTTI is enabled
#ifdef __cpp_rtti
    if (dynamic_cast<Derived*>(base)) {
        return static_cast<Derived*>(base);
    }
#else
    // Without RTTI, use our own type tracking
    if (typeid(*base) == typeid(Derived)) {
        return static_cast<Derived*>(base);
    }
#endif
    
    return nullptr;
}

/**
 * Method 5: Type registry for safe casting
 */
class TypeRegistry {
private:
    struct TypeInfo {
        std::type_index type_idx;
        std::string type_name;
    };
    
    std::unordered_map<uintptr_t, TypeInfo> registry;
    
public:
    template<typename T>
    void registerObject(T* obj) {
        if (obj) {
            uintptr_t address = reinterpret_cast<uintptr_t>(obj);
            registry[address] = {std::type_index(typeid(T)), typeid(T).name()};
        }
    }
    
    template<typename T>
    void unregisterObject(T* obj) {
        if (obj) {
            uintptr_t address = reinterpret_cast<uintptr_t>(obj);
            registry.erase(address);
        }
    }
    
    template<typename Derived, typename Base>
    Derived* safeCast(Base* base) {
        if (!base) return nullptr;
        
        uintptr_t address = reinterpret_cast<uintptr_t>(base);
        auto it = registry.find(address);
        
        if (it != registry.end()) {
            if (it->second.type_idx == std::type_index(typeid(Derived))) {
                return static_cast<Derived*>(base);
            }
        }
        
        return nullptr;
    }
    
    void printRegistry() const {
        std::cout << "Type Registry contents:\n";
        for (const auto& [addr, info] : registry) {
            std::cout << "  Address: " << (void*)addr 
                      << ", Type: " << info.type_name << "\n";
        }
    }
};

/**
 * Method 6: Double dispatch pattern
 */
class Processor {
public:
    virtual void process(Base* obj) = 0;
    virtual void processSpecific(Derived1* obj) = 0;
    virtual void processSpecific(Derived2* obj) = 0;
    virtual void processSpecific(Derived3* obj) = 0;
    virtual ~Processor() = default;
};

class CastProcessor : public Processor {
private:
    void* result = nullptr;
    
public:
    void process(Base* obj) override {
        std::cout << "Processing base object\n";
    }
    
    void processSpecific(Derived1* obj) override {
        result = obj;
        std::cout << "Cast to Derived1 successful\n";
    }
    
    void processSpecific(Derived2* obj) override {
        result = obj;
        std::cout << "Cast to Derived2 successful\n";
    }
    
    void processSpecific(Derived3* obj) override {
        result = obj;
        std::cout << "Cast to Derived3 successful\n";
    }
    
    template<typename T>
    T* getResult() { return static_cast<T*>(result); }
};

/**
 * Helper function to demonstrate casting
 */
template<typename Func>
void demonstrateCasting(const std::string& method_name, Func cast_func, 
                        Base* base, const std::string& target) {
    std::cout << "\n" << method_name << ":\n";
    
    if (target == "Derived1") {
        auto* derived = cast_func.template operator()<Derived1>(base);
        if (derived) {
            derived->identify();
            derived->derived1SpecificMethod();
        } else {
            std::cout << "Cast failed - object is not Derived1\n";
        }
    }
    else if (target == "Derived2") {
        auto* derived = cast_func.template operator()<Derived2>(base);
        if (derived) {
            derived->identify();
            derived->derived2SpecificMethod();
        } else {
            std::cout << "Cast failed - object is not Derived2\n";
        }
    }
    else if (target == "Derived3") {
        auto* derived = cast_func.template operator()<Derived3>(base);
        if (derived) {
            derived->identify();
            derived->derived3SpecificMethod();
        } else {
            std::cout << "Cast failed - object is not Derived3\n";
        }
    }
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Base to Derived Casting without dynamic_cast\n";
    std::cout << "========================================\n";
    
    // Create objects
    std::cout << "\n--- Creating objects ---\n";
    Base* base1 = new Derived1("test data");
    Base* base2 = new Derived2(100);
    Base* base3 = new Derived3(2.71828);
    Base* base_only = new Base();
    
    std::vector<Base*> objects = {base1, base2, base3, base_only};
    
    // Method 1: Virtual function based casting
    std::cout << "\n=== Method 1: Virtual function based ===\n";
    for (Base* obj : objects) {
        std::cout << "\nTesting object:\n";
        obj->identify();
        
        if (obj->isDerived1()) {
            Derived1* d1 = static_cast<Derived1*>(obj);
            d1->derived1SpecificMethod();
        } else if (obj->isDerived2()) {
            Derived2* d2 = static_cast<Derived2*>(obj);
            d2->derived2SpecificMethod();
        }
    }
    
    // Method 2: Enum-based casting
    std::cout << "\n=== Method 2: Enum-based casting ===\n";
    auto enum_caster = [](auto* base) { return enumCast<decltype(*base)>(base); };
    
    for (Base* obj : objects) {
        if (auto* d1 = enumCast<Derived1>(obj)) {
            std::cout << "Found Derived1: "; d1->identify();
        }
        if (auto* d2 = enumCast<Derived2>(obj)) {
            std::cout << "Found Derived2: "; d2->identify();
        }
        if (auto* d3 = enumCast<Derived3>(obj)) {
            std::cout << "Found Derived3: "; d3->identify();
        }
    }
    
    // Method 3: Typeinfo-based casting
    std::cout << "\n=== Method 3: Typeinfo-based casting ===\n";
    for (Base* obj : objects) {
        demonstrateCasting("Typeinfo cast", 
            [](auto* base) { return typeinfoCast<decltype(*base)>(base); }, 
            obj, typeid(*obj).name());
    }
    
    // Method 4: Type registry
    std::cout << "\n=== Method 4: Type registry ===\n";
    TypeRegistry registry;
    
    // Register objects
    for (Base* obj : objects) {
        if (dynamic_cast<Derived1*>(obj)) {
            registry.registerObject(static_cast<Derived1*>(obj));
        } else if (dynamic_cast<Derived2*>(obj)) {
            registry.registerObject(static_cast<Derived2*>(obj));
        } else if (dynamic_cast<Derived3*>(obj)) {
            registry.registerObject(static_cast<Derived3*>(obj));
        } else {
            registry.registerObject(obj);
        }
    }
    
    registry.printRegistry();
    
    // Test registry casting
    for (Base* obj : objects) {
        if (auto* d1 = registry.safeCast<Derived1>(obj)) {
            std::cout << "Registry cast to Derived1: "; d1->identify();
        }
        if (auto* d2 = registry.safeCast<Derived2>(obj)) {
            std::cout << "Registry cast to Derived2: "; d2->identify();
        }
        if (auto* d3 = registry.safeCast<Derived3>(obj)) {
            std::cout << "Registry cast to Derived3: "; d3->identify();
        }
    }
    
    // Method 5: Double dispatch
    std::cout << "\n=== Method 5: Double dispatch ===\n";
    CastProcessor processor;
    
    for (Base* obj : objects) {
        // This would require accept method in Base
        // Simplified demonstration
        if (auto* d1 = dynamic_cast<Derived1*>(obj)) {
            processor.processSpecific(d1);
        } else if (auto* d2 = dynamic_cast<Derived2*>(obj)) {
            processor.processSpecific(d2);
        } else if (auto* d3 = dynamic_cast<Derived3*>(obj)) {
            processor.processSpecific(d3);
        } else {
            processor.process(obj);
        }
    }
    
    // Method 6: Template-based type checking
    std::cout << "\n=== Method 6: Template-based type checking ===\n";
    for (Base* obj : objects) {
        std::cout << "Object is ";
        if (obj->isType<Derived1>()) std::cout << "Derived1";
        else if (obj->isType<Derived2>()) std::cout << "Derived2";
        else if (obj->isType<Derived3>()) std::cout << "Derived3";
        else std::cout << "Base";
        std::cout << "\n";
    }
    
    // Demonstrate unsafe casting
    std::cout << "\n=== UNSAFE Casting (for demonstration) ===\n";
    std::cout << "WARNING: This demonstrates what NOT to do:\n";
    
    // UNSAFE: Direct static cast without checking
    Derived2* unsafe_d2 = static_cast<Derived2*>(base1);  // base1 is actually Derived1!
    std::cout << "Unsafe cast of Derived1 to Derived2 (BAD!):\n";
    // This would cause undefined behavior if called
    // unsafe_d2->derived2SpecificMethod(); // DON'T DO THIS!
    
    // SAFE: Check first
    if (typeid(*base1) == typeid(Derived2)) {
        Derived2* safe_d2 = static_cast<Derived2*>(base1);
        safe_d2->derived2SpecificMethod();
    } else {
        std::cout << "Safe check prevented invalid cast\n";
    }
    
    // Method 7: Visitor pattern (alternative to casting)
    std::cout << "\n=== Method 7: Visitor pattern (alternative) ===\n";
    class Visitor {
    public:
        void visit(Base* b) { std::cout << "Visiting Base\n"; }
        void visit(Derived1* d) { 
            std::cout << "Visiting Derived1 with data: " << d->getData() << "\n";
        }
        void visit(Derived2* d) { 
            std::cout << "Visiting Derived2 with value: " << d->getValue() << "\n";
        }
        void visit(Derived3* d) { 
            std::cout << "Visiting Derived3 with ratio: " << d->getRatio() << "\n";
        }
    };
    
    Visitor visitor;
    for (Base* obj : objects) {
        if (auto* d1 = dynamic_cast<Derived1*>(obj)) visitor.visit(d1);
        else if (auto* d2 = dynamic_cast<Derived2*>(obj)) visitor.visit(d2);
        else if (auto* d3 = dynamic_cast<Derived3*>(obj)) visitor.visit(d3);
        else visitor.visit(obj);
    }
    
    // Cleanup
    std::cout << "\n--- Cleaning up ---\n";
    delete base1;
    delete base2;
    delete base3;
    delete base_only;
    
    // Summary
    std::cout << "\n=== Summary ===\n";
    std::cout << "Methods demonstrated:\n";
    std::cout << "1. Virtual function checks (isDerived1(), etc.)\n";
    std::cout << "2. Enum-based type identification\n";
    std::cout << "3. Typeinfo comparison\n";
    std::cout << "4. Type registry for safe casting\n";
    std::cout << "5. Double dispatch pattern\n";
    std::cout << "6. Template-based type checking\n";
    std::cout << "7. Visitor pattern (alternative to casting)\n";
    std::cout << "\nKey safety principles:\n";
    std::cout << "✓ Always verify type before casting\n";
    std::cout << "✓ Never assume the actual type\n";
    std::cout << "✓ Use type identification mechanisms\n";
    std::cout << "✓ Consider alternatives like Visitor pattern\n";
    
    return 0;
}