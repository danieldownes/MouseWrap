namespace MultimonitorEdges
{
    [TestFixture]
    public class ContourTests
    {
        [Test]
        public void GetContour_TwoRectangles_T_ShapedContour()
        {
            var rectangles = new List<Rectangle>
            {
                new Rectangle(0, 10, 0, 30),
                new Rectangle(10, 20, -40, 50)
            };

            var contourEdges = Contour.GetContour(rectangles);

            var expectedEdges = new List<Edge>
            {
                new Edge(0, 10, -40, 50),
                new Edge(10, 20, -40, 50),
                new Edge(0, 20, 0, 30),
                new Edge(0, 20, -40, -40),
                new Edge(0, 20, 50, 50)
            };

            CollectionAssert.AreEquivalent(expectedEdges, contourEdges);
        }

        [Test]
        public void GetContour_TwoRectangles_L_ShapedContour()
        {
            var rectangles = new List<Rectangle>
            {
                new Rectangle(0, 10, 0, 30),
                new Rectangle(10, 20, 20, 40)
            };

            var contourEdges = Contour.GetContour(rectangles);

            var expectedEdges = new List<Edge>
            {
                new Edge(0, 10, 0, 30),
                new Edge(10, 20, 20, 40),
                new Edge(0, 20, 0, 0),
                new Edge(0, 20, 30, 30),
                new Edge(0, 0, 0, 30),
                new Edge(20, 20, 20, 40)
            };

            CollectionAssert.AreEquivalent(expectedEdges, contourEdges);
        }

        [Test]
        public void GetContour_TwoRectangles_NoIntersection()
        {
            var rectangles = new List<Rectangle>
            {
                new Rectangle(0, 10, 0, 30),
                new Rectangle(20, 30, 20, 40)
            };

            var contourEdges = Contour.GetContour(rectangles);

            var expectedEdges = new List<Edge>
        {
            new Edge(0, 10, 0, 30),
            new Edge(20, 30, 20, 40),
            new Edge(0, 10, 30, 30),
            new Edge(0, 0, 0, 30),
            new Edge(10, 10, 0, 30),
            new Edge(20, 20, 20, 40),
            new Edge(30, 30, 20, 40),
            new Edge(10, 10, 30, 30)
        };

            CollectionAssert.AreEquivalent(expectedEdges, contourEdges);
        }

        [Test]
        public void GetContour_OneRectangle_RectangleEdges()
        {
            var rectangle = new Rectangle(0, 10, 0, 20);

            var contourEdges = Contour.GetContour(new List<Rectangle> { rectangle });

            var expectedEdges = new List<Edge>
            {
                new Edge(0, 10, 0, 0),
                new Edge(0, 10, 20, 20),
                new Edge(0, 0, 0, 20),
                new Edge(10, 10, 0, 20)
            };

            CollectionAssert.AreEquivalent(expectedEdges, contourEdges);
        }
    }
}