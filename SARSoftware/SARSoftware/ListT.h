//-------------------------------------------------
// List and Node: using singly linked list of Nodes
//-------------------------------------------------

#ifndef ListTH
#define ListTH

#include <assert.h> // for assert
//#include <cstdlib> // for nullptr

//------------------------------------------------------------------------------
// Node
//
//   T is the type parameter, it is the type that will be stored in the Node.
//
//   This is stored in the same file as List because List directly accesses its
//   private attributes. It could be rewritten using public functions to access
//   private data, or by making Node a "friend" class of List.
//
//   This is fine for now though!
//
//------------------------------------------------------------------------------

template <class T>
class Node
{
public:
  Node(const T& item); // Node constructor
  T item_;             // the data item in the node
  Node<T>* next_;      // pointer to the next node (initialised to nullptr)
};

// Node implementation
// -------------------
template <class T>
Node<T>::Node(const T& item) : item_(item), next_(nullptr)
{
}

//-------------------------------------------------------------------------------
// List
//
//   ITEM is the type parameter, it is the type that will be managed by the List.
//
//-------------------------------------------------------------------------------

template <class T>
class List
{
public:
  List();                                  // default constructor
  List(const List<T>&);                    // copy constructor
  List(const T&);                          // constructor to make a list from an item
  ~List();                                 // destructor
  List<T>& operator = (const List<T>&);    // copy assignment operator
  bool operator == (const List<T>&) const; // compare with content of another list
  bool isEmpty() const;                    // check if list is empty
  const T first() const;                   // return first item (MUST be non-empty)
  const T last() const;                    // return last item (MUST be non-empty)
  const List<T> tail() const;              // return tail (MUST be non-empty)
  void addInFront(const T&);               // insert an item in front
  void addAtEnd(const T&);                 // insert an item at end
  void deleteFirst();                      // delete first item (MUST be non-empty)
  void deleteOne(const T&);                // delete first occurrence of item
  int length() const;                      // return length
  bool contains(const T&) const;           // check if an item is in list
  T operator[] (int) const;
private:
  Node<T>* head_;                          // point onto first item (nullptr if empty)
  Node<T>* end() const;                    // return address of last item (nullptr if empty)
  void destroy();                          // delete all items in list
  void copy(const List<T>&);               // make a deep copy of the list
};

// List implementation
// -------------------
template <class T>
List<T>::List() : head_(nullptr)
{
}

template <class T>
List<T>::List(const T& item) : head_(new Node<T>(item))
{
  assert(head_ != nullptr);
}

template <class T>
List<T>::List(const List<T>& other) : head_(nullptr)
{
  copy(other);
}

template <class T>
List<T>::~List()
{
  destroy();
}

// public operations
// -----------------
/*template <class T>
T List<T>::operator[] (int index) const;
{
  List<T> temp = *this;
  for (int i = 0; i < index; i++) {
    temp.deleteFirst();
  }
  return temp.first();
}*/
template <class T>
List<T>& List<T>::operator = (const List<T>& rhs)
{
  if (&rhs != this)
  {
    List<T> temp = rhs;
    Node<T> *pn = head_;
    head_ = temp.head_;
    temp.head_ = pn;
  }

  return *this;
}

template <class T>
bool List<T>::operator == (const List<T>& rhs) const
{
  if (isEmpty() && rhs.isEmpty())
  {
    return true;
  }
  else
  {
    if (!isEmpty() && !rhs.isEmpty() && first() == rhs.first() && tail() == rhs.tail())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

template <class T>
bool List<T>::isEmpty() const
{
  return head_ == nullptr;
}

template <class T>
const T List<T>::first() const
{
  assert(head_ != nullptr);
  return head_->item_;
}

template <class T>
const T List<T>::last() const
{
  assert(head_ != nullptr);
  return end()->item_;
}

template <class T>
const List<T> List<T>::tail() const
{
  assert(head_ != nullptr);
  List<T> tailList = *this;
  tailList.deleteFirst();
  return tailList;
}

template <class T>
void List<T>::addInFront(const T& val)
{
  Node<T>* pn = new Node<T>(val);
  assert(pn != nullptr);

  pn->next_ = head_;
  head_ = pn;
}

template <class T>
void List<T>::addAtEnd(const T& val)
{
  Node<T>* pn = new Node<T>(val);
  assert(pn != nullptr);

  if (head_ == nullptr)
  {
    head_ = pn;
  }
  else
  {
    end()->next_ = pn;
  }
}

template <class T>
void List<T>::deleteFirst()
{
  assert(head_ != nullptr);
  Node<T>* pn = head_;
  head_ = head_->next_;
  delete pn;
}

template <class T>
void List<T>::deleteOne(const T& item)
{
  if (isEmpty()) return;

  if (first() == item)
  {
    deleteFirst();
  }
  else
  {
    List<T> temp = tail();
    temp.deleteOne(item);
    temp.addInFront(first());
    *this = temp;
  }
}

template <class T>
int List<T>::length() const
{
  int size = 0;

  for (Node<T>* pn = head_; pn != nullptr; pn = pn->next_)
  {
    size++;
  }

  return size;
}

template <class T>
bool List<T>::contains(const T& item) const
{
  for (Node<T>* pn = head_; pn != nullptr; pn = pn->next_)
  {
    if (pn->item_ == item)
    {
      return true;
    }
  }

  return false;
}

template<class T>
T List<T>::operator[](int index) const
{
  List<T> temp = *this;
  for (int i = 0; i < index; i++) {
    temp.deleteFirst();
  }
  return temp.first();
}

// private (support) operations
// ----------------------------
template <class T>
Node<T>* List<T>::end() const
{
  Node<T>* plast = head_;

  while (plast->next_ != nullptr)
  {
    plast = plast->next_;
  }

  return plast;
}

template <class T>
void List<T>::destroy()
{
  while (head_ != nullptr)
  {
    deleteFirst();
  }
}

template <class T>
void List<T>::copy(const List<T>& other)
{
  assert(head_ == nullptr);

  if (other.head_ == nullptr) return;

  head_ = new Node<T>(other.head_->item_);
  assert(head_ != nullptr);

  Node<T>* pnew = head_;
  for (Node<T>* porig = other.head_->next_; porig != nullptr; porig = porig->next_)
  {
    pnew->next_ = new Node<T>(porig->item_);
    assert(pnew->next_ != nullptr);
    pnew = pnew->next_;
  }
}

#endif
