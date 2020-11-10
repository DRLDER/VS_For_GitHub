using Microsoft.VisualBasic;
using System;
using A;


namespace A
{
    class Name
    {
        
        public void PrintName(string name)
        {
            Console.Write("My name is:[" + name + "]!");
        }

        
    }
    
}

namespace B
{
    class Program
    {

        static void Main(string[] args)
        {

            Name nameplace=new Name();
            Console.Write("Please write your name:\n");
            string name = Console.ReadLine();
            nameplace.PrintName(name);
            Console.ReadLine();
        }
    }
}
