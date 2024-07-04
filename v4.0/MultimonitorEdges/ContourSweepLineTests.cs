namespace MultimonitorEdges.Tests
{
    [TestFixture]
    public class ContourSweepLineTests
    {
        [Test]
        public void GetContour_SingleRectangle_ReturnsFourEdges()
        {
            var rectangles = new List<Rectangle> { new Rectangle(0, 10, 0, 10) };
            var contour = ContourSweepLine.GetContour(rectangles);

            Assert.AreEqual(4, contour.Count);
        }

        [Test]
        public void GetContour_TwoOverlappingRectangles_ReturnsCorrectContour()
        {
            var rectangles = new List<Rectangle>
            {
                new Rectangle(0, 10, 0, 10),
                new Rectangle(5, 15, 5, 15)
            };
            var contour = ContourSweepLine.GetContour(rectangles);

            // The contour should have 8 edges
            Assert.AreEqual(8, contour.Count);

            // Check for specific edges (you may need to adjust these based on your implementation)
            Assert.Contains(new Edge(0, 0, 0, 10, true), contour);
            Assert.Contains(new Edge(0, 15, 0, 0, true), contour);
            Assert.Contains(new Edge(15, 15, 5, 15, false), contour);
            Assert.Contains(new Edge(5, 15, 15, 15, false), contour);
        }

        [Test]
        public void GetContour_TwoAdjacentRectangles_ReturnsMergedContour()
        {
            var rectangles = new List<Rectangle>
            {
                new Rectangle(0, 10, 0, 10),
                new Rectangle(10, 20, 0, 10)
            };
            var contour = ContourSweepLine.GetContour(rectangles);

            // The contour should have 4 edges
            Assert.AreEqual(4, contour.Count);

            // Check for specific edges
            Assert.Contains(new Edge(0, 0, 0, 10, true), contour);
            Assert.Contains(new Edge(0, 20, 0, 0, true), contour);
            Assert.Contains(new Edge(20, 20, 0, 10, false), contour);
            Assert.Contains(new Edge(0, 20, 10, 10, false), contour);
        }
    }
}