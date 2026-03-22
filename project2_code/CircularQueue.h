#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <stdexcept>

template <typename T>
class CircularQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* front;
    Node* rear;
    int size;

public:
    CircularQueue() : front(nullptr), rear(nullptr), size(0) {}

    ~CircularQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    bool isEmpty() const { return size == 0; }
    int getSize() const { return size; }

    void enqueue(const T& val) {
        Node* newNode = new Node(val);
        if (isEmpty()) {
            front = newNode;
            rear = newNode;
            newNode->next = front;
        } else {
            rear->next = newNode;
            rear = newNode;
            rear->next = front;
        }
        size++;
    }

    T peek() const {
        if (isEmpty()) throw std::runtime_error("Queue is empty.");
        return front->data;
    }

    T dequeue() {
        if (isEmpty()) throw std::runtime_error("Queue is empty.");
        T val = front->data;
        if (size == 1) {
            delete front;
            front = nullptr;
            rear = nullptr;
        } else {
            Node* temp = front;
            front = front->next;
            rear->next = front;
            delete temp;
        }
        size--;
        return val;
    }

    void rotate() {
        if (isEmpty()) return;
        T val = dequeue();
        enqueue(val);
    }

    // Get element at index (0 = front)
    T& getAt(int idx) {
        if (idx < 0 || idx >= size) throw std::out_of_range("Index out of range.");
        Node* curr = front;
        for (int i = 0; i < idx; i++) curr = curr->next;
        return curr->data;
    }

    // Move element at index to front of queue
    void moveToFront(int idx) {
        if (idx <= 0 || idx >= size) return; // already front or invalid
        // Find node before target
        Node* prev = front;
        for (int i = 0; i < idx - 1; i++) prev = prev->next;
        Node* target = prev->next;

        // Splice target out
        prev->next = target->next;
        if (target == rear) rear = prev;

        // Insert at front
        target->next = front;
        front = target;
        rear->next = front;
    }

    // Traverse: void func(T&, int index, bool isFront)
    template <typename Func>
    void traverse(Func func) {
        if (isEmpty()) return;
        Node* curr = front;
        int idx = 0;
        do {
            func(curr->data, idx, (curr == front));
            curr = curr->next;
            idx++;
        } while (curr != front);
    }
};

#endif
