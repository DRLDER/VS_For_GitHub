using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using WebApplicationmvcTest.Models;

namespace WebApplicationmvcTest.Controllers
{
    public class TestController : Controller
    {
        //GET: Test
        public ActionResult TestFunc(string name, int ID = 1)
        {
            
            ViewBag.Message = "This is a test!"+ HttpUtility.HtmlEncode("Hello " + name + ", ID: " + ID);
            ViewData["1"] = "This is a messageTest!";
            ViewData["data"] = new List<modtest>()
            {
                new modtest
               {
                 ID = 1,
                 Name = "唐僧",
                 Age = 34,
                 Sex = "男",
                 Email = "747976523@qq.com"
               },
               new modtest
               {
                 ID = 2,
                 Name = "孙悟空",
                 Age = 635,
                 Sex = "男",
                 Email = "sunwukong@163.com"
               },
               new modtest
               {
                 ID = 3,
                 Name = "白骨精",
                 Age = 4532,
                 Sex = "女",
                 Email = "74345523@qq.com"
               }
            };
            return View();
        }
       
    }
}