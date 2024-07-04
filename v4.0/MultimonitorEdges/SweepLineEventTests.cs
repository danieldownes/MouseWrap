namespace MultimonitorEdges.Tests
{
    [TestFixture]
    public class SweepLineEventTests
    {
        [Test]
        public void Constructor_SetsPropertiesCorrectly()
        {
            var edge = new Edge(0, 0, 0, 1, true);
            var evt = new SweepLineEvent(5, edge, true);

            Assert.AreEqual(5, evt.X);
            Assert.AreEqual(edge, evt.Edge);
            Assert.IsTrue(evt.IsStart);
        }

        [Test]
        public void CompareTo_SameX_StartBeforeEnd()
        {
            var edge = new Edge(0, 0, 0, 1, true);
            var evt1 = new SweepLineEvent(5, edge, true);
            var evt2 = new SweepLineEvent(5, edge, false);

            Assert.Less(evt1.CompareTo(evt2), 0);
        }

        [Test]
        public void CompareTo_DifferentX_SmallerFirst()
        {
            var edge = new Edge(0, 0, 0, 1, true);
            var evt1 = new SweepLineEvent(3, edge, true);
            var evt2 = new SweepLineEvent(5, edge, true);

            Assert.Less(evt1.CompareTo(evt2), 0);
        }

        [Test]
        public void CompareTo_SameXAndType_ReturnsZero()
        {
            var edge = new Edge(0, 0, 0, 1, true);
            var evt1 = new SweepLineEvent(5, edge, true);
            var evt2 = new SweepLineEvent(5, edge, true);

            Assert.AreEqual(0, evt1.CompareTo(evt2));
        }
    }
}