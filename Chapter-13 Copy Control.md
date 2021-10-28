# 第13章 拷贝控制

一个类通过定义五种特殊的成员函数来控制对象的拷贝、移动、赋值和销毁操作。

- 拷贝构造函数（copy constructor）
- 拷贝赋值运算符（copy-assignment operator）
- 移动构造函数（move constructor）
- 移动赋值运算符（move-assignment operator）
- 析构函数（destructor）

这些操作统称为拷贝控制操作（copy control）。

在定义任何类时，拷贝控制操作都是必要部分。

## 1. 拷贝、赋值与销毁（Copy，Assign，and Destroy）

### 1.1 拷贝构造函数（The Copy Constructor）

如果一个构造函数的第一个参数是自身类类型的引用（几乎总是`const`引用），且任何额外参数都有默认值，则此构造函数是拷贝构造函数。

```c++
class Foo
{
public:
    Foo();   // default constructor
    Foo(const Foo&);   // copy constructor，const的，可以有其他参数，但额外参数应该有默认值
    // ...
};
```

由于拷贝构造函数在一些情况下会被<u>隐式使用</u>，因此通常不会声明为`explicit`的。

**合成拷贝构造函数（synthesized copy constructor）**：如果类未定义自己的拷贝构造函数，编译器会为类合成一个。一般情况下，合成拷贝构造函数会将其参数的非`static`成员逐个拷贝到正在创建的对象中。

```c++
class Sales_data
{
public:
    // other members and constructors as before
    // declaration equivalent to the synthesized copy constructor
    Sales_data(const Sales_data&);
private:
    std::string bookNo;
    int units_sold = 0;
    double revenue = 0.0;
};

// equivalent to the copy constructor that would be synthesized for Sales_data
Sales_data::Sales_data(const Sales_data &orig):
    bookNo(orig.bookNo),    // uses the string copy constructor
    units_sold(orig.units_sold),    // copies orig.units_sold
    revenue(orig.revenue)   // copies orig.revenue
    { } // empty bod
```

使用<u>直接初始化</u>时，实际上是要求编译器按照函数匹配规则来选择与实参最匹配的构造函数。

使用<u>拷贝初始化</u>时，是要求编译器将右侧运算对象拷贝到正在创建的对象中，如果需要的话还要进行类型转换。

```c++
string dots(10, '.');   // direct initialization
string s(dots);         // direct initialization
string s2 = dots;       // copy initialization
string null_book = "9-999-99999-9";    // copy initialization
string nines = string(100, '9');       // copy initialization
```

拷贝初始化通常使用拷贝构造函数来完成。但如果一个类拥有移动构造函数，则拷贝初始化有时会使用移动构造函数而非拷贝构造函数来完成。

发生拷贝初始化的情况：

- 用`=`定义变量。
- 将对象作为实参传递给非引用类型的形参。
- 从返回类型为非引用类型的函数返回对象。
- 用花括号列表初始化数组中的元素或聚合类中的成员。???

当传递一个实参或者从函数返回一个值时，不能隐式使用`explicit`构造函数。比如下面的vector的接受单一大小参数的构造函数是explicit的，必须像最后一行那样显示调用。

```c++
vector<int> v1(10);     // ok: direct initialization
vector<int> v2 = 10;    // error: constructor that takes a size is explicit
void f(vector<int>);    // f's parameter is copy initialized
f(10);      // error: can't use an explicit constructor to copy an argument
f(vector<int>(10));     // ok: directly construct a temporary vector from an int
```

### 1.2 拷贝赋值运算符（The Copy-Assignment Operator）

赋值是指一个对象被定义后，再给该对象赋予其他同类型对象的值

重载运算符（overloaded operator）的参数表示运算符的运算对象。

如果一个运算符是成员函数，则其左侧运算对象会绑定到隐式的`this`参数上。

赋值运算符通常应该返回一个指向其左侧运算对象的引用（与内置类型的赋值保持一致）。

```c++
class Foo
{
public:
    Foo& operator=(const Foo&);  // assignment operator
    // ...
};
```

标准库通常要求保存在容器中的类型要具有赋值运算符，且其返回值是左侧运算对象的引用。

如果类未定义自己的拷贝赋值运算符，编译器会为类合成一个。一般情况下，合成拷贝赋值运算符（synthesized copy-assignment operator）会将其右侧运算对象的非`static`成员逐个赋值给左侧运算对象的对应成员，之后返回左侧运算对象的引用。

```c++
// 等价于合成拷贝赋值运算符
Sales_data& Sales_data::operator=(const Sales_data &rhs)
{
    bookNo = rhs.bookNo;    // calls the string::operator=
    units_sold = rhs.units_sold;    // uses the built-in int assignment
    revenue = rhs.revenue;  // uses the built-in double assignment
    return *this;   // return a reference to this object
}
```

### 1.3 析构函数（The Destructor）

析构函数负责<u>释放对象使用的资源</u>，<u>并销毁对象的非`static`数据成员</u>。

析构函数的名字由波浪号`~`接类名构成，它没有返回值，也不接受参数。

```c++
class Foo
{
public:
    ~Foo(); // destructor
    // ...
};
```

由于析构函数不接受参数，所以它不能被重载。

如果类未定义自己的析构函数，编译器会为类合成一个。<u>合成析构函数（synthesized destructor）的函数体为空。</u>

**析构函数完成什么工作**：

析构函数首先执行函数体，然后再销毁数据成员，销毁部分是隐式的。在整个对象销毁过程中，析构函数体是作为成员销毁步骤之外的另一部分而进行的。成员按照初始化顺序的逆序销毁。(作为对比，构造函数的成员初始化实在函数体执行之前完成的，且按照它们在类中出现的顺序进行初始化)

隐式销毁一个内置指针类型的成员不会`delete`它所指向的对象，即析构阶段指针变量被销毁了，但该指针处的内容还是存在的（智能指针有其析构函数，在析构阶段会被自动销毁）

无论何时一个对象被销毁，都会自动调用其析构函数。

当指向一个对象的引用或指针离开作用域时，该对象的析构函数不会执行。

再次声明：析构函数体自身并不直接销毁成员，而是在析构函数体之后隐含的析构阶段中被销毁的。

### 1.4 三/五法则（The Rule of Three/Five）

需要析构函数的类一般也需要拷贝和赋值操作。

需要拷贝操作的类一般也需要赋值操作，反之亦然。

### 1.5 使用=default（Using =default）

可以通过将拷贝控制成员定义为`=default`来显式地要求编译器生成合成版本。

```c++
class Sales_data
{
public:
    // copy control; use defaults
    Sales_data() = default;
    Sales_data(const Sales_data&) = default;
    ~Sales_data() = default;
    // other members as before
};
```

在类内使用`=default`修饰成员声明时，合成的函数是隐式内联的。

如果不希望合成的是内联函数，应该只对成员的类外定义使用`=default`（比如拷贝赋值运算符那样）

只能对具有合成版本的成员函数使用`=default`（即，默认构造函数或拷贝控制成员）

### 1.6 阻止拷贝（Preventing Copies）

大多数类应该定义默认构造函数、拷贝构造函数和拷贝赋值运算符，无论是显式地还是隐式地。

在C++11新标准中，将拷贝构造函数和拷贝赋值运算符定义为**删除的函数**（deleted function）可以阻止类对象的拷贝。删除的函数是一种虽然进行了声明，但是却不能以任何方式使用的函数。定义删除函数的方式是在函数的形参列表后面添加`=delete`。

```c++
struct NoCopy
{
    NoCopy() = default; // use the synthesized default constructor
    NoCopy(const NoCopy&) = delete; // no copy
    NoCopy &operator=(const NoCopy&) = delete; // no assignment
    ~NoCopy() = default; // use the synthesized destructor
    // other members
};
```

`=delete`和`=default`有两点不同：

- `=delete`可以对任何函数使用；`=default`只能对具有合成版本的函数使用。
- `=delete`必须出现在函数第一次声明的地方；`=default`既能出现在类内（内联），也能出现在类外（不是内联）。

析构函数不能是删除的函数。对于析构函数被删除的类型，不能定义该类型的变量或者释放指向该类型动态分配对象的指针。

如果一个类中有数据成员不能默认构造、拷贝或销毁，则对应的合成拷贝控制成员将被定义为删除的。(如果)

在旧版本的C++标准中，类通过将拷贝构造函数和拷贝赋值运算符声明为`private`成员来阻止类对象的拷贝。在新标准中建议使用`=delete`而非`private`。

### 【补充内容】

**1、C++ 类中特殊成员变量（常量、静态、引用）的初始化方法**

参考：https://blog.csdn.net/lanchunhui/article/details/50772363

- 引用（&）
- 常量（const）
- 静态（static）
  - 静态常量（static const）
    - 整数类型（integral types：int、char、bool、）
    - 非整数类型

1. 常量（const）和引用（&）必须通过**初始化列表**进行初始化（<u>**理解初始化列表**：初始化列表采用括号对数据成员进行初始化，采用括号赋值的方法，括号赋值用在变量的初始化阶段而不能用在定义之后的赋值</u>）
2. 静态成员变量的初始化有些特别，是在类外初始化且不能在带有 static 关键字
3. 带有类内初始值设定项的成员必须为常量（且为integral types）

横向来看，不同数据类型的初始化方式分别为：

- 引用：初始化参数列表，构造函数的形参也必须是引用类型（如果形参不是引用，类中引用成员就会指向临时形参变量，所以必须是外面传进来的引用）
- const
  - 类内初始化值设定（如果为integral types）
  - 初始化参数列表

- static：
  - 类外初始化，且不再带有 static 关键字
  - 类内初始化设置，必须为const，且integral
  - 不可通过构造函数的初始化参数列表进行初始化
    - static 不属于某一特定的类实例，而是属于类，显然不能通过构造函数的初始化参数列表进行 static 变量的初始化（<u>静态成员属于类作用域，但不属于类对象实例</u>）

**2、理解类的定义**：类定义中不能进行初始化，因为类的定义只是一个声明，并没有分配真正空间，因此变量是不存在的，因此是不能赋值的。所以对于类实例中数据成员的<u>初始化和内存分配</u>，要用好各种构造函数。

**3、C++中构造函数，拷贝构造函数和赋值函数的区别**

**构造函数**：当创建一个类的对象时，它被调用来对类的数据成员进行初始化和分配内存。

**拷贝构造函数**：是一种特殊的构造函数，用基于同一类的一个对象构造和初始化另一个对象。

在C++中，3种对象需要复制，此时拷贝构造函数会被调用

- 一个对象以值传递的方式传入函数体

- 一个对象以值传递的方式从函数返回

- 一个对象需要通过另一个对象进行初始化

**赋值函数**：当一个类的对象向该类的另一个对象赋值时，就会用到该类的赋值函数

```c++
// “拷贝构造函数”和“赋值函数”的区别
class A;
// 拷贝构造函数，这里b对象是不存在的，是用a 对象来构造和初始化b的！！
A a;
A b(a); // 拷贝构造函数来创建对象b
A b=a;  // 拷贝构造函数来创建对象b

// 赋值函数，这里a,b对象是已经存在的，是用a 对象来赋值给b的！！
A a;
A b;
b=a;  // 将a赋值b
```



## 2. 拷贝控制和资源管理（Copy Control and Resource Management）

通常，管理类外资源的类必须定义拷贝控制成员。拷贝操作有两种，使得类的行为像一个值或者像一个指针。

类行为像值，当拷贝该类的对象时，副本和原对象完全独立，改变副本不会改变原对象。

类行为像指针，当拷贝该类的对象时，副本和原对象使用相同的底层数据，改变副本也会改变原对象，即该类所有的对象共享状态

### 2.1 行为像值的类（Classes That Act Like Values）

```C++
class HasPtr
{
public:
    HasPtr(const std::string &s = std::string()):
        ps(new std::string(s)), i(0) { }
    // each HasPtr has its own copy of the string to which ps points
    HasPtr(const HasPtr &p):
        ps(new std::string(*p.ps)), i(p.i) { }
    HasPtr& operator=(const HasPtr &);
    ~HasPtr() { delete ps; }

private:
    std::string *ps;
    int i;
};
```

编写赋值运算符时有两点需要注意：

- 即使将一个对象赋予它自身，赋值运算符也能正确工作。

  ```c++
  // 这样编写赋值运算符是错误的，处理不了自赋值
  HasPtr& HasPtr::operator=(const HasPtr &rhs)
  {
      delete ps;   // frees the string to which this object points
      // if rhs and *this are the same object, we're copying from deleted memory!
      ps = new string(*(rhs.ps));
      i = rhs.i;
      return *this;
  }
  ```

- 赋值运算符通常结合了拷贝构造函数和析构函数的工作。

  编写赋值运算符时，一个好的方法是先将右侧运算对象拷贝到一个局部临时对象中。拷贝完成后，就可以安全地销毁左侧运算对象的现有成员了。

  ```c++
  HasPtr& HasPtr::operator=(const HasPtr &rhs)
  {
      auto newp = new string(*rhs.ps);    // copy the underlying string
      delete ps;   // free the old memory
      ps = newp;   // copy data from rhs into this object
      i = rhs.i;
      return *this;   // return this object
  }
  ```

### 2.2 定义行为像指针的类（Defining Classes That Act Like Pointers）

对于行为像指针的类，需要定义拷贝构造函数和拷贝赋值运算符，来拷贝指针成员而不是内容

另一个类展现类似指针的行为的最好方法是使用shared_ptr来管理类中的资源，如果希望直接管理资源，可以使用**引用计数**：

- 每个构造函数（除了拷贝构造函数）要创建一个引用计数
- 对于拷贝构造函数：递增共享的计数器
- 对于析构函数：递减计数器，如果计数器变为0，析构函数销毁状态
- 对于拷贝赋值运算符：递增右边运算对象的计数器，递减左边运算对象的计数器

可将计数器保存在动态内存中，拷贝指向计数器的指针

```c++
class HasPtr
{
public:
    // constructor allocates a new string and a new counter, which it sets to 1
    HasPtr(const std::string &s = std::string()):
        ps(new std::string(s)), i(0), use(new std::size_t(1)) {}
    // copy constructor copies all three data members and increments the counter
    HasPtr(const HasPtr &p):
        ps(p.ps), i(p.i), use(p.use) { ++*use; }
    HasPtr& operator=(const HasPtr&);
    ~HasPtr();

private:
    std::string *ps;
    int i;
    std::size_t *use; // member to keep track of how many objects share *ps
};
```

析构函数释放内存前应该判断是否还有其他对象指向这块内存。

```c++
HasPtr::~HasPtr()
{
    if (--*use == 0)
    {   // if the reference count goes to 0
        delete ps;   // delete the string
        delete use;  // and the counter
    }
}

// 赋值运算符必须考虑自赋值：先递增rhs中的计数器，再递减左侧运算对象
HasPtr& HasPtr::operator=(const HasPtr &rhs)
{
    ++*rhs.use;    // increment the use count of the right-hand operand
    if (--*use == 0)
    {   // then decrement this object's counter
        delete ps; // if no other users
        delete use; // free this object's allocated members
    }
    ps = rhs.ps;    // copy data from rhs into this object
    i = rhs.i;
    use = rhs.use;
    return *this;   // return this object
}
```

## 3. 交换操作（Swap）

与拷贝控制成员不同，`swap`函数并不是必要的。对于分配了资源的类，定义`swap`函数是一种很重要的优化手段。

如果一个类定义了自己的`swap`函数，算法将使用自定义版本，否则将使用标准库定义的`swap`。

```c++
class HasPtr
{
    friend void swap(HasPtr&, HasPtr&);  // friend，以便访问类内数据成员
    // other members as in § 13.2.1 (p. 511)
};

// 由于`swap`函数的存在就是为了优化代码，所以一般将其声明为内联函数。
inline void swap(HasPtr &lhs, HasPtr &rhs) 
{
    using std::swap; // 数据成员是内置类型，内置类型没有特定版本的swap，所以会调用std::swap
    swap(lhs.ps, rhs.ps);   // swap the pointers, not the string data
    swap(lhs.i, rhs.i);     // swap the int members
}
```

**swap和std::swap**：一些算法在交换两个元素时会调用`swap`函数，其中每个`swap`调用都应该是未加限定的。如果存在类型特定的`swap`版本，其匹配程度会优于*std*中定义的版本（假定作用域中有`using`声明）。

```c++
// 下面错误
void swap(Foo &lhs, Foo &rhs)
{
    // WRONG: this function uses the library version of swap, not the HasPtr version
    // 标准swap对HasPtr管理的string进行了不必要拷贝
    std::swap(lhs.h, rhs.h);
    // swap other members of type Foo
}

// 下面正确
void swap(Foo &lhs, Foo &rhs)
{
    using std::swap; 
    swap(lhs.h, rhs.h);  // uses the HasPtr version of swap
    // swap other members of type Foo
}
```

**在赋值运算符中使用swap**：定义了`swap`的类通常用`swap`来实现赋值运算符。在这种版本的赋值运算符中，右侧运算对象以值方式传递，然后将左侧运算对象与右侧运算对象的副本进行交换（拷贝并交换，copy and swap）。

```c++
// note rhs is passed by value, which means the HasPtr copy constructor
// copies the string in the right-hand operand into rhs
HasPtr& HasPtr::operator=(HasPtr rhs)
{
    // swap the contents of the left-hand operand with the local variable rhs
    swap(*this, rhs);   // rhs now points to the memory this object had used
    return *this;       // rhs is destroyed, which deletes the pointer in rhs
}
```

这种方式可以正确处理自赋值情况。它通过在改变左侧运算对象之前拷贝右侧运算对象保证自赋值情况。

## 4. 拷贝控制示例（A Copy-Control Example）

拷贝赋值运算符通常结合了拷贝构造函数和析构函数的工作。在这种情况下，公共部分应该放在`private`的工具函数中完成。

## 5. 动态内存管理类（Classes That Manage Dynamic Memory）

移动构造函数通常是将资源从给定对象“移动”而不是拷贝到正在创建的对象中。

**STL:: allocator之deallocate & destory的区别与联系**：

deallocate：

```c++
	template <class T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);    //为什么不用 delete [] ?  ,operator delete 区别于 delete 
		                             //operator delete  是一个底层操作符
	}
```

destory：

```c++
	template <class T>
	inline void _destory(T *ptr)
	{
		ptr->~T();
	}
```

destory()负责调用类型的析构函数，销毁相应内存上的内容（但销毁后内存地址仍保留）

destroy()是将已经被构造的空间(已有值) 析构成没有被使用的空间放回内存池



deallocate()是将分配出的空间(没有被使用的空间) 放回主存。

deallocate()负责释放内存（此时相应内存中的值在此之前应调用destory销毁，将内存地址返回给系统，代表这部分地址使用引用-1）

## 6. 对象移动（Moving Objects）

某些情况下，一个对象拷贝后就立即被销毁了，此时移动而非拷贝对象会大幅度提高性能。

在旧版本的标准库中，容器所能保存的类型必须是可拷贝的。但在新标准中，<u>可以用容器保存不可拷贝，但可移动的类型</u>。

标准库容器、`string`和`shared_ptr`类既支持移动也支持拷贝。IO类和`unique_ptr`类可以移动但不能拷贝。

### 6.1 右值引用（Rvalue Reference）

为了支持移动操作，C++11引入了右值引用类型。右值引用就是必须绑定到右值的引用。可以通过`&&`来获得右值引用。

```c++
int i = 42;
int &r = i;         // ok: r refers to i
int &&rr = i;       // error: cannot bind an rvalue reference to an
int &r2 = i * 42;   // error: i * 42 is an rvalue
const int &r3 = i * 42;    // ok: we can bind a reference to const to an rvalue
int &&rr2 = i * 42;        // ok: bind rr2 to the result of the multiplication
```

返回左值引用：赋值、下标、解引用和前置递增/递减运算符

返回右值引用：算术、关系、位和后置递增/递减运算符（可以将一个const的左值引用绑定到这类表达式上，如`const int &r3 = i * 42;`）

左值持久，右值短暂：右值引用只能绑定到即将被销毁，并且没有其他用户的临时对象上。所以使用右值引用的代码可以自由地接管所引用对象的资源。

**变量是左值**，所以不能将一个右值引用直接绑定到一个变量上，即使这个变量的类型是右值引用也不行。

```c++
int &&rr1 = 42;     // ok: literals are rvalues
int &&rr2 = rr1;    // error: the expression rr1 is an lvalue!
```

**标准库move函数**：调用`move`函数可以<u>获得绑定在左值上的右值引用</u>，此函数定义在头文件*utility*中。move表示——我们有一个左值，但希望像右值一样处理它。

```c++
int &&rr3 = std::move(rr1); // 除了对rr1赋值或销毁外，我们将不再使用它，即我们可以销毁一个移后源对象，也可以赋予新值，但不能使用一个移后源对象的值
```

调用`move`函数的代码应该使用`std::move`而非`move`，这样做可以避免潜在的名字冲突。

### 6.2 移动构造函数和移动赋值运算符（Move Constructor and Move Assignment）

移动构造函数的第一个参数是该类类型的右值引用，其他任何额外参数都必须有默认值。

除了完成资源移动，移动构造函数还必须确保移后源对象是可以安全销毁的。

在函数的形参列表后面添加关键字`noexcept`可以指明该函数不会抛出任何异常。

对于构造函数，`noexcept`位于形参列表和初始化列表开头的冒号之间。在类的头文件声明和定义中（如果定义在类外）都应该指定`noexcept`。

```c++
class StrVec
{
public:
    StrVec(StrVec&&) noexcept;  // move constructor
    // other members as before
};

StrVec::StrVec(StrVec &&s) noexcept : /* member initializers */
{ /* constructor body */ }
```

标准库容器能对异常发生时其自身的行为提供保障。虽然移动操作通常不抛出异常，但抛出异常也是允许的。为了安全起见，除非容器确定元素类型的移动操作不会抛出异常，否则在重新分配内存的过程中，它就必须使用拷贝而非移动操作。

不抛出异常的移动构造函数和移动赋值运算符必须标记为`noexcept`。

在移动操作之后，移后源对象必须保持有效的、可销毁的状态，但是用户不能使用它的值。

```c++
StrVec &StrVec::operator=(StrVec &&rhs) noexcept
{
    // direct test for self-assignment
    if (this != &rhs)
    {
        free();     // free existing elements
        elements = rhs.elements;    // take over resources from rhs
        first_free = rhs.first_free;
        cap = rhs.cap;
        // leave rhs in a destructible state
        rhs.elements = rhs.first_free = rhs.cap = nullptr;
    }
    return *this;
}
```

只有当一个类没有定义任何拷贝控制成员，且类的每个非`static`数据成员都可以移动时，编译器才会为类合成移动构造函数和移动赋值运算符。编译器可以移动内置类型的成员。如果一个成员是类类型，且该类有对应的移动操作，则编译器也能移动该成员。

```c++
// the compiler will synthesize the move operations for X and hasX
struct X
{
    int i;   // built-in types can be moved
    std::string s;   // string defines its own move operations
};

struct hasX
{
    X mem; // X has synthesized move operations
};

X x, x2 = std::move(x);         // uses the synthesized move constructor
hasX hx, hx2 = std::move(hx);   // uses the synthesized move constructor
```

与拷贝操作不同，移动操作永远不会被隐式定义为删除的函数。但如果显式地要求编译器生成`=default`的移动操作，且编译器不能移动全部成员，则移动操作会被定义为删除的函数。

定义了移动构造函数或移动赋值运算符的类必须也定义自己的拷贝操作，否则这些成员会被默认地定义为删除的函数。

如果一个类有可用的拷贝构造函数而没有移动构造函数，则其对象是通过拷贝构造函数来“移动”的，即使调用`move`函数时也是如此。拷贝赋值运算符和移动赋值运算符的情况类似。

```c++
class Foo
{
public:
    Foo() = default;
    Foo(const Foo&);    // copy constructor
    // other members, but Foo does not define a move constructor
};

Foo x;
Foo y(x);   // copy constructor; x is an lvalue
Foo z(std::move(x));    // copy constructor, because there is no move constructor
```

使用非引用参数的单一赋值运算符可以实现拷贝赋值和移动赋值两种功能。依赖于实参的类型，左值被拷贝，右值被移动。

```c++
// assignment operator is both the move- and copy-assignment operator
HasPtr& operator=(HasPtr rhs)
{
    swap(*this, rhs);
    return *this;
}

hp = hp2;   // hp2 is an lvalue; copy constructor used to copy hp2
hp = std::move(hp2);    // move constructor moves hp2
```

建议将五个拷贝控制成员当成一个整体来对待。如果一个类需要任何一个拷贝操作，它就应该定义所有五个操作。

移动赋值运算符可以直接检查自赋值情况。

C++11标准库定义了移动迭代器（move iterator）适配器。一个移动迭代器通过改变给定迭代器的解引用运算符的行为来适配此迭代器。移动迭代器的解引用运算符返回一个右值引用。

调用`make_move_iterator`函数能将一个普通迭代器转换成移动迭代器。原迭代器的所有其他操作在移动迭代器中都照常工作。

最好不要在移动构造函数和移动赋值运算符这些类实现代码之外的地方随意使用`move`操作。

### 6.3 右值引用和成员函数（Rvalue References and Member Functions）

区分移动和拷贝的重载函数通常有一个版本接受一个`const T&`参数，另一个版本接受一个`T&&`参数（*T*为类型）。

```c++
void push_back(const X&);   // copy: binds to any kind of X
void push_back(X&&);        // move: binds only to modifiable rvalues of type X
```

有时可以对右值赋值：

```c++
string s1, s2;
s1 + s2 = "wow!";
```

在旧标准中，没有办法阻止这种使用方式。为了维持向下兼容性，新标准库仍然允许向右值赋值。但是可以在自己的类中阻止这种行为，规定左侧运算对象（即`this`指向的对象）必须是一个左值。

在非`static`成员函数的形参列表后面添加引用限定符（reference qualifier）可以指定`this`的左值/右值属性。引用限定符可以是`&`或者`&&`，分别表示`this`可以指向一个左值或右值对象。引用限定符必须同时出现在函数的声明和定义中。

```c++
class Foo
{
public:
    Foo &operator=(const Foo&) &; // may assign only to modifiable lvalues
    // other members of Foo
};

Foo &Foo::operator=(const Foo &rhs) &
{
    // do whatever is needed to assign rhs to this object
    return *this;
}
```

一个非`static`成员函数可以同时使用`const`和引用限定符，此时引用限定符跟在`const`限定符之后。

```c++
class Foo
{
public:
    Foo someMem() & const;      // error: const qualifier must come first
    Foo anotherMem() const &;   // ok: const qualifier comes first
};
```

引用限定符也可以区分成员函数的重载版本。

```c++
class Foo
{
public:
    Foo sorted() &&;        // may run on modifiable rvalues
    Foo sorted() const &;   // may run on any kind of Foo
};

retVal().sorted();   // retVal() is an rvalue, calls Foo::sorted() &&
retFoo().sorted();   // retFoo() is an lvalue, calls Foo::sorted() const &
```

如果一个成员函数有引用限定符，则具有相同参数列表的所有重载版本都必须有引用限定符。

```c++
class Foo
{
public:
    Foo sorted() &&;
    Foo sorted() const;    // error: must have reference qualifier
    // Comp is type alias for the function type
    // that can be used to compare int values
    using Comp = bool(const int&, const int&);
    Foo sorted(Comp*);  // ok: different parameter list
};
```