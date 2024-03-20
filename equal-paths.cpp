#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool checker(Node *root, int level, int& leafLevel)
{
  // checks edge case
  if (root == nullptr)
  {
      return true;
  }

  // if at leaf node
  if (root->right == nullptr && root->left == nullptr)
  {
      // on first case, we set the leafLevel for subsequent checking
      if (leafLevel == 0)
      {
          leafLevel = level;
          return true;
      }

      else if (level == leafLevel)
      {
          return true;
      }

      else
      {
          return false;
      }
  }

  // recursively checks each leaf
  if (checker(root->left, level + 1, leafLevel) && checker(root->right, level + 1, leafLevel))
  {
      return true;
  }

  else
  {
      return false;
  }
}

bool equalPaths(Node *root)
{
  // Add your code below
  int level = 0;
  int leafLevel = 0;
  return checker(root, level, leafLevel);
}
