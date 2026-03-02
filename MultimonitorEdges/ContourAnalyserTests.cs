namespace MultimonitorEdges.Tests
{
    [TestFixture]
    public class ContourAnalyserTests
    {
        [Test]
        public void GetOppositePointInContour_PointOnEdge_ReturnsOppositePoint()
        {
            var edges = new List<Edge>
            {
                new Edge(0, 0, 0, 10, true),
                new Edge(0, 10, 0, 0, true),
                new Edge(10, 10, 0, 10, false),
                new Edge(0, 10, 10, 10, false)
            };

            var point = new Point(0, 5);
            var oppositePoint = ContourAnalyser.GetOppositePointInContour(point, edges);

            Assert.IsNotNull(oppositePoint);
            Assert.That(oppositePoint.Value, Is.EqualTo(new Point(10, 5)));
        }

        [Test]
        public void GetOppositePointInContour_PointNotOnEdge_ReturnsNull()
        {
            var edges = new List<Edge>
            {
                new Edge(0, 0, 0, 10, true),
                new Edge(0, 10, 0, 0, true),
                new Edge(10, 10, 0, 10, false),
                new Edge(0, 10, 10, 10, false)
            };

            var point = new Point(5, 5);
            var oppositePoint = ContourAnalyser.GetOppositePointInContour(point, edges);

            Assert.IsNull(oppositePoint);
        }

        [Test]
        public void GetOppositePointInContour_ComplexContour_ReturnsCorrectOppositePoint()
        {
            var edges = new List<Edge>
            {
                new Edge(0, 0, 0, 10, true),
                new Edge(0, 10, 0, 0, true),
                new Edge(10, 10, 0, 5, false),
                new Edge(10, 15, 5, 5, true),
                new Edge(15, 15, 5, 10, false),
                new Edge(10, 15, 10, 10, false),
                new Edge(0, 10, 10, 10, false)
            };

            var point = new Point(0, 6);
            var oppositePoint = ContourAnalyser.GetOppositePointInContour(point, edges);

            Assert.IsNotNull(oppositePoint);
            //See: https://grantwinney.com/5-reasons-to-use-nunits-constraint-model/
            Assert.That(oppositePoint.Value.X, Is.EqualTo(15));
            Assert.That(oppositePoint.Value.Y, Is.EqualTo(6));
        }
    }
}