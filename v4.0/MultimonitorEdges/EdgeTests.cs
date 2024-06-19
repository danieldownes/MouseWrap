namespace MultimonitorEdges;

[TestFixture]
public class EdgeTests
{

    [Test]
    public void Edge_Intersects_VerticalEdgesIntersecting_ReturnsTrue()
    {
        var edge1 = new Edge(5, 5, 0, 10);
        var edge2 = new Edge(5, 5, 5, 15);

        Assert.IsTrue(edge1.Intersects(edge2));
        Assert.IsTrue(edge2.Intersects(edge1));
    }

    [Test]
    public void Edge_Intersects_HorizontalEdgesIntersecting_ReturnsTrue()
    {
        var edge1 = new Edge(0, 10, 5, 5);
        var edge2 = new Edge(5, 15, 5, 5);

        Assert.IsTrue(edge1.Intersects(edge2));
        Assert.IsTrue(edge2.Intersects(edge1));
    }

    [Test]
    public void Edge_Intersects_NonIntersectingEdges_ReturnsFalse()
    {
        var edge1 = new Edge(0, 10, 0, 10);
        var edge2 = new Edge(20, 30, 20, 30);

        Assert.IsFalse(edge1.Intersects(edge2));
        Assert.IsFalse(edge2.Intersects(edge1));
    }
}
