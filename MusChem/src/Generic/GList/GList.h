#ifndef H_GENERIC_GLIST_H
#define H_GENERIC_GLIST_H


// Project include files
#include "GUtils.h"

// System include files
#include <stdio.h>
#include <string.h>


// Node structure:
// Structure representing one node in the linked list.
template <typename T>
struct GNode
{
    T data;
    GNode<T> *p_next;
    char nodeId[GENERAL_STRING_SIZE];

    GNode()
    {
        for(int iChar = 0; iChar < GENERAL_STRING_SIZE; iChar++)
        {
            nodeId[iChar] = '\0';
        }
    }
};

// List class:
// Class representing a single linked list
template <typename T>
class GList
{
    // Foreach macro
    #define foreach(p_node, list) \
       for(p_node = list.getHead(); p_node != NULL; p_node = p_node->p_next)

    public:
        // Constuctor
        GList()
        {
            _p_head = NULL;
            _nNodes = 0;
        }

        // Destructor
        ~GList()
        {
            GNode<T> *p_node = _p_head;
            GNode<T> *p_delNode;

            while(p_node != NULL)
            {
                p_delNode = p_node;
                p_node = p_node->p_next;
                delete p_delNode;
            }

            _nNodes = 0;
        }

        // Add element at beginning
        void addBegin(T data, const char *nodeId)
        {
            // Copy data
            GNode<T> *p_newNode = new GNode<T>;
            p_newNode->data = data;
            p_newNode->p_next = _p_head;
            strncpy(p_newNode->nodeId, nodeId, GENERAL_STRING_SIZE);

            // Add at beginning
            _p_head = p_newNode;
            
            _nNodes++;
        }

        // Add element at end
        void addEnd(T data, const char *nodeId)
        {
            // Copy data
            GNode<T> *p_newNode = new GNode<T>;
            p_newNode->data = data;
            p_newNode->p_next = NULL;
            strncpy(p_newNode->nodeId, nodeId, GENERAL_STRING_SIZE);

            // Find end
            GNode<T> *p_driver = _p_head;
            GNode<T> *p_prevDriver;
            while(p_driver != NULL)
            {
                p_prevDriver = p_driver;
                p_driver = p_driver->p_next;
            }

            // Add at end
            p_prevDriver->p_next = p_newNode;
            
            _nNodes++;
        }

        T find(const char *nodeId)
        {
            for(GNode<T> *p_node = _p_head;
                p_node != NULL;
                p_node = p_node->p_next)
            {
                if(strcmp(nodeId, p_node->nodeId) == 0)
                {
                    return p_node->data;
                }
            }

            return NULL;
        }

        void remove(const char *nodeId)
        {
            GNode<T> *p_driver = _p_head;
            GNode<T> **p_preDriver = &_p_head;

            while(p_driver != NULL &&
                strcmp(nodeId, (*p_preDriver)->nodeId) != 0)
            {
                p_driver = p_driver->p_next;
                p_preDriver = &(*p_preDriver)->p_next;
            }

            *p_preDriver = p_driver->p_next;

            delete p_driver;

            _nNodes--;
        }

        GNode<T> *getHead()
        {
            return _p_head;
        }

    private:
        // Head pointer
        GNode<T> *_p_head;

        // Number of nodes
        int _nNodes;

};


#endif // H_GENERIC_GLIST_H
